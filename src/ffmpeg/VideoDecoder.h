#pragma once

#include "FFmpegEngine.h"
#include <QString>
#include <QImage>
#include <QVector>
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

namespace cinemastudio {

class VideoDecoder {
public:
    VideoDecoder();
    ~VideoDecoder();

    bool open(const QString& filePath);
    void close();
    bool isOpen() const { return m_formatCtx != nullptr; }

    // Get frame at specific time
    VideoFrame getFrameAtTime(double timeSeconds);
    
    // Get sequential frames
    bool getNextFrame(VideoFrame& outFrame);
    
    // Get all frames (for rendering)
    QVector<VideoFrame> getFramesInRange(double startTime, double endTime, int targetFps = 30);

    // Thumbnails
    QImage generateThumbnail(double timeSeconds, int width = 160, int height = 90);
    QVector<QImage> generateThumbnails(int count, int width = 160, int height = 90);

    // Info
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    double getDuration() const { return m_duration; }
    double getFps() const { return m_fps; }
    int getVideoStreamIndex() const { return m_videoStreamIndex; }

private:
    bool initVideoStream();
    VideoFrame convertFrameToRGB(const VideoFrame& srcFrame);

    AVFormatContext* m_formatCtx = nullptr;
    AVCodecContext* m_codecCtx = nullptr;
    AVPacket* m_packet = nullptr;
    AVFrame* m_frame = nullptr;
    AVFrame* m_frameRGB = nullptr;
    SwsContext* m_swsCtx = nullptr;
    
    int m_videoStreamIndex = -1;
    int m_width = 0;
    int m_height = 0;
    double m_duration = 0.0;
    double m_fps = 0.0;
    double m_currentTime = 0.0;
    
    QString m_filePath;
};

} // namespace cinemastudio
