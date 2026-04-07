#include "VideoEncoder.h"
#include "FFmpegEngine.h"
#include <QDebug>
#include <QProcess>
#include <QRegularExpression>
#include <QTimer>

namespace cinemastudio {

VideoEncoder::VideoEncoder(QObject* parent) : QObject(parent) {
}

VideoEncoder::~VideoEncoder() {
    stopEncoding();
}

bool VideoEncoder::startEncoding(const QString& inputFile, const QString& outputFile,
                                  const FFmpegEngine::EncodeSettings& settings) {
    if (m_isEncoding) {
        qWarning() << "Already encoding";
        return false;
    }

    m_isEncoding = true;
    m_cancelled = false;

    m_encodingThread = new QThread();
    moveToThread(m_encodingThread);

    connect(m_encodingThread, &QThread::started, this, [this, inputFile, outputFile, settings]() {
        encodingThread(inputFile, outputFile, settings);
    });

    m_encodingThread->start();
    return true;
}

void VideoEncoder::stopEncoding() {
    m_cancelled = true;
    m_isEncoding = false;
    if (m_encodingThread) {
        m_encodingThread->quit();
        m_encodingThread->wait();
        delete m_encodingThread;
        m_encodingThread = nullptr;
    }
}

void VideoEncoder::encodingThread(const QString& inputFile, const QString& outputFile,
                                   const FFmpegEngine::EncodeSettings& settings) {
    auto startTime = std::chrono::steady_clock::now();

    QString ffmpegCmd = "ffmpeg";
    QStringList args;
    
    args << "-y";
    args << "-i" << inputFile;
    args << "-c:v" << settings.videoCodec;
    args << "-preset" << settings.preset;
    args << "-crf" << QString::number(settings.crf);
    args << "-c:a" << settings.audioCodec;
    args << "-b:a" << QString::number(settings.audioBitrate / 1000) + "k";
    args << "-vf" << QString("scale=%1:%2").arg(settings.resolution.width()).arg(settings.resolution.height());
    args << "-r" << QString::number(settings.fps);
    args << outputFile;

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(ffmpegCmd, args);
    
    if (!process.waitForStarted()) {
        emit encodingError("Failed to start FFmpeg");
        m_isEncoding = false;
        return;
    }

    double totalDuration = 0;
    QRegularExpression durationRegex("Duration: (\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2})");
    QRegularExpression timeRegex("time=(\\d{2}):(\\d{2}):(\\d{2})\\.(\\d{2})");

    while (process.state() != QProcess::NotRunning) {
        if (m_cancelled) {
            process.kill();
            emit encodingFinished(false);
            m_isEncoding = false;
            return;
        }

        if (process.waitForReadyRead(100)) {
            QByteArray output = process.readAllStandardOutput();
            QString outputStr = QString::fromUtf8(output);

            // Parse duration
            if (totalDuration == 0) {
                QRegularExpressionMatch match = durationRegex.match(outputStr);
                if (match.hasMatch()) {
                    int hours = match.captured(1).toInt();
                    int minutes = match.captured(2).toInt();
                    int seconds = match.captured(3).toInt();
                    totalDuration = hours * 3600 + minutes * 60 + seconds;
                }
            }

            // Parse current time
            QRegularExpressionMatch match = timeRegex.match(outputStr);
            if (match.hasMatch() && totalDuration > 0) {
                int hours = match.captured(1).toInt();
                int minutes = match.captured(2).toInt();
                int seconds = match.captured(3).toInt();
                double currentTime = hours * 3600 + minutes * 60 + seconds;
                
                int progress = static_cast<int>((currentTime / totalDuration) * 100);
                double elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - startTime).count();
                
                emit encodingProgress(progress, elapsed);
            }
        }

        if (process.waitForFinished(100)) {
            break;
        }
    }

    bool success = process.exitCode() == 0;
    if (!success) {
        emit encodingError(process.readAllStandardError());
    } else {
        emit encodingFinished(true);
    }

    m_isEncoding = false;
}

} // namespace cinemastudio
