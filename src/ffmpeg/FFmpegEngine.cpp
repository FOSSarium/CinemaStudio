#include "FFmpegEngine.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <chrono>

namespace cinemastudio {

VideoFrame::VideoFrame() = default;

VideoFrame::~VideoFrame() { free(); }

void VideoFrame::allocate(int w, int h, AVPixelFormat fmt) {
  free();
  width = w;
  height = h;
  format = fmt;
  int ret = av_image_alloc(data, linesize, w, h, fmt, 32);
  if (ret < 0) {
    qCritical() << "Failed to allocate video frame:"
                << FFmpegEngine::getErrorString(ret);
  }
}

void VideoFrame::free() {
  if (data[0]) {
    av_freep(&data[0]);
  }
  for (int i = 0; i < 4; i++) {
    data[i] = nullptr;
    linesize[i] = 0;
  }
}

VideoFrame VideoFrame::clone() const {
  VideoFrame cloned;
  cloned.allocate(width, height, format);
  for (int i = 0; i < 4; i++) {
    if (data[i] && linesize[i]) {
      memcpy(cloned.data[i], data[i], linesize[i] * height);
    }
  }
  cloned.pts = pts;
  return cloned;
}

FFmpegEngine::FFmpegEngine() = default;
FFmpegEngine::~FFmpegEngine() = default;

void FFmpegEngine::initialize() {
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 9, 100)
  av_register_all();
#endif
  avformat_network_init();
}

void FFmpegEngine::cleanup() { avformat_network_deinit(); }

MediaInfo FFmpegEngine::getMediaInfo(const QString &filePath) {
  MediaInfo info;
  info.filePath = filePath;

  AVFormatContext *formatCtx = nullptr;
  int ret = avformat_open_input(&formatCtx, filePath.toUtf8().constData(),
                                nullptr, nullptr);
  if (ret < 0) {
    qCritical() << "Failed to open file:" << getErrorString(ret);
    return info;
  }

  ret = avformat_find_stream_info(formatCtx, nullptr);
  if (ret < 0) {
    qCritical() << "Failed to find stream info:" << getErrorString(ret);
    avformat_close_input(&formatCtx);
    return info;
  }

  info.duration = formatCtx->duration / (double)AV_TIME_BASE;

  for (unsigned int i = 0; i < formatCtx->nb_streams; i++) {
    AVStream *stream = formatCtx->streams[i];
    AVCodecParameters *codecParams = stream->codecpar;

    if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO &&
        info.videoStreamIndex < 0) {
      info.videoStreamIndex = i;
      info.hasVideo = true;
      info.resolution = QSize(codecParams->width, codecParams->height);
      info.videoCodec = codecParams->codec_id;

      AVRational fps = stream->r_frame_rate;
      if (fps.den > 0) {
        info.fps = fps.num / (double)fps.den;
      }
    } else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO &&
               info.audioStreamIndex < 0) {
      info.audioStreamIndex = i;
      info.hasAudio = true;
      info.audioCodec = codecParams->codec_id;
      info.audioSampleRate = codecParams->sample_rate;
      info.audioChannels = codecParams->ch_layout.nb_channels;
    }
  }

  avformat_close_input(&formatCtx);
  return info;
}

bool FFmpegEngine::encode(const QString &inputFile, const QString &outputFile,
                          const EncodeSettings &settings,
                          ProgressCallback callback) {
  m_cancelled = false;
  auto startTime = std::chrono::steady_clock::now();

  // For now, use FFmpeg CLI for encoding (more reliable)
  QString ffmpegCmd = "ffmpeg";
  QStringList args;

  args << "-y"; // Overwrite output
  args << "-i" << inputFile;
  args << "-c:v" << settings.videoCodec;
  args << "-preset" << settings.preset;
  args << "-crf" << QString::number(settings.crf);
  args << "-c:a" << settings.audioCodec;
  args << "-b:a" << QString::number(settings.audioBitrate / 1000) + "k";
  args << "-vf"
       << QString("scale=%1:%2")
              .arg(settings.resolution.width())
              .arg(settings.resolution.height());
  args << "-r" << QString::number(settings.fps);
  args << outputFile;

  QProcess process;
  process.start(ffmpegCmd, args);

  if (!process.waitForStarted()) {
    qCritical() << "Failed to start FFmpeg";
    return false;
  }

  while (process.state() != QProcess::NotRunning) {
    if (m_cancelled) {
      process.kill();
      return false;
    }

    if (process.waitForFinished(500)) {
      break;
    }

    if (callback) {
      auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::steady_clock::now() - startTime)
                         .count();
      callback(50, elapsed); // Estimate
    }
  }

  int exitCode = process.exitCode();
  return exitCode == 0;
}

void FFmpegEngine::cancelEncoding() { m_cancelled = true; }

QString FFmpegEngine::formatTime(double seconds) {
  int hrs = static_cast<int>(seconds) / 3600;
  int mins = (static_cast<int>(seconds) % 3600) / 60;
  int secs = static_cast<int>(seconds) % 60;
  int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);

  if (hrs > 0) {
    return QString("%1:%2:%3.%4")
        .arg(hrs, 2, 10, QChar('0'))
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0'));
  }
  return QString("%1:%2.%3")
      .arg(mins, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'))
      .arg(ms, 3, 10, QChar('0'));
}

QString FFmpegEngine::getErrorString(int error) {
  char errorBuf[AV_ERROR_MAX_STRING_SIZE];
  av_strerror(error, errorBuf, AV_ERROR_MAX_STRING_SIZE);
  return QString(errorBuf);
}

} // namespace cinemastudio
