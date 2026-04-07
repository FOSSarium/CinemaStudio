#pragma once

#include <QString>
#include <QSize>
#include <functional>
#include <memory>
#include <atomic>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

namespace cinemastudio {

struct VideoFrame {
    uint8_t* data[4] = {nullptr};
    int linesize[4] = {0};
    int width = 0;
    int height = 0;
    double pts = 0.0;
    AVPixelFormat format = AV_PIX_FMT_RGB24;
    
    VideoFrame();
    ~VideoFrame();
    void allocate(int w, int h, AVPixelFormat fmt = AV_PIX_FMT_RGB24);
    void free();
    VideoFrame clone() const;
};

struct AudioFrame {
    std::vector<float> samples;
    double pts = 0.0;
    int sampleRate = 0;
    int channels = 0;
};

struct MediaInfo {
    QString filePath;
    double duration = 0.0;
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    QSize resolution;
    double fps = 0.0;
    AVCodecID videoCodec = AV_CODEC_ID_NONE;
    AVCodecID audioCodec = AV_CODEC_ID_NONE;
    int audioSampleRate = 0;
    int audioChannels = 0;
    bool hasVideo = false;
    bool hasAudio = false;
};

using ProgressCallback = std::function<void(int progress, double elapsed)>;

class FFmpegEngine {
public:
    FFmpegEngine();
    ~FFmpegEngine();

    // Initialize FFmpeg
    static void initialize();
    static void cleanup();

    // Media info
    static MediaInfo getMediaInfo(const QString& filePath);

    // Encoding settings
    struct EncodeSettings {
        QString outputFormat = "mp4";
        QString videoCodec = "libx264";
        QString audioCodec = "aac";
        int videoBitrate = 8000000;  // 8 Mbps
        int audioBitrate = 192000;   // 192 kbps
        int fps = 30;
        QSize resolution = QSize(1920, 1080);
        int audioSampleRate = 44100;
        int audioChannels = 2;
        int crf = 23;  // Constant Rate Factor (18-28, lower = better)
        QString preset = "medium";  // ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow
    };

    // Encoding
    bool encode(const QString& inputFile, const QString& outputFile, 
                const EncodeSettings& settings, ProgressCallback callback = nullptr);
    void cancelEncoding();

    // Utility
    static QString formatTime(double seconds);
    static QString getErrorString(int error);

private:
    std::atomic<bool> m_cancelled{false};
};

} // namespace cinemastudio
