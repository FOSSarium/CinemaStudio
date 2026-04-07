#pragma once

#include "FFmpegEngine.h"
#include <QString>
#include <QObject>
#include <QThread>
#include <atomic>

namespace cinemastudio {

class VideoEncoder : public QObject {
    Q_OBJECT
public:
    VideoEncoder(QObject* parent = nullptr);
    ~VideoEncoder();

    bool startEncoding(const QString& inputFile, const QString& outputFile,
                       const FFmpegEngine::EncodeSettings& settings);
    void stopEncoding();
    bool isEncoding() const { return m_isEncoding; }

signals:
    void encodingProgress(int percent, double elapsed);
    void encodingFinished(bool success);
    void encodingError(const QString& error);

private:
    void encodingThread(const QString& inputFile, const QString& outputFile,
                        const FFmpegEngine::EncodeSettings& settings);

    std::atomic<bool> m_isEncoding{false};
    std::atomic<bool> m_cancelled{false};
    QThread* m_encodingThread = nullptr;
};

} // namespace cinemastudio
