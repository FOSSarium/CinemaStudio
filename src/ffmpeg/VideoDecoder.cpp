#include "VideoDecoder.h"
#include <QDebug>
#include <algorithm>

namespace cinemastudio {

VideoDecoder::VideoDecoder() = default;

VideoDecoder::~VideoDecoder() {
    close();
}

bool VideoDecoder::open(const QString& filePath) {
    close();
    m_filePath = filePath;

    int ret = avformat_open_input(&m_formatCtx, filePath.toUtf8().constData(), nullptr, nullptr);
    if (ret < 0) {
        qCritical() << "Failed to open video:" << FFmpegEngine::getErrorString(ret);
        return false;
    }

    ret = avformat_find_stream_info(m_formatCtx, nullptr);
    if (ret < 0) {
        qCritical() << "Failed to find stream info:" << FFmpegEngine::getErrorString(ret);
        close();
        return false;
    }

    if (!initVideoStream()) {
        close();
        return false;
    }

    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    m_frameRGB = av_frame_alloc();

    m_duration = m_formatCtx->duration / (double)AV_TIME_BASE;
    
    AVRational fps = m_formatCtx->streams[m_videoStreamIndex]->r_frame_rate;
    if (fps.den > 0) {
        m_fps = fps.num / (double)fps.den;
    }

    return true;
}

void VideoDecoder::close() {
    if (m_swsCtx) {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
    }
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
    }
    if (m_packet) {
        av_packet_free(&m_packet);
    }
    if (m_frame) {
        av_frame_free(&m_frame);
    }
    if (m_frameRGB) {
        av_frame_free(&m_frameRGB);
    }

    m_videoStreamIndex = -1;
    m_width = 0;
    m_height = 0;
    m_duration = 0.0;
    m_fps = 0.0;
    m_currentTime = 0.0;
}

bool VideoDecoder::initVideoStream() {
    for (unsigned int i = 0; i < m_formatCtx->nb_streams; i++) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }

    if (m_videoStreamIndex < 0) {
        qCritical() << "No video stream found";
        return false;
    }

    AVCodecParameters* codecParams = m_formatCtx->streams[m_videoStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        qCritical() << "Unsupported codec";
        return false;
    }

    m_codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codecCtx, codecParams);

    int ret = avcodec_open2(m_codecCtx, codec, nullptr);
    if (ret < 0) {
        qCritical() << "Failed to open codec:" << FFmpegEngine::getErrorString(ret);
        return false;
    }

    m_width = m_codecCtx->width;
    m_height = m_codecCtx->height;

    return true;
}

VideoFrame VideoDecoder::getFrameAtTime(double timeSeconds) {
    if (!isOpen()) return VideoFrame();

    // Seek to time
    int64_t timestamp = timeSeconds * AV_TIME_BASE;
    int ret = avformat_seek_file(m_formatCtx, -1, INT64_MIN, timestamp, INT64_MAX, 0);
    if (ret < 0) {
        qCritical() << "Seek failed:" << FFmpegEngine::getErrorString(ret);
        return VideoFrame();
    }

    avcodec_flush_buffers(m_codecCtx);
    m_currentTime = timeSeconds;

    // Decode frames until we get a video frame
    while (av_read_frame(m_formatCtx, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            ret = avcodec_send_packet(m_codecCtx, m_packet);
            if (ret >= 0) {
                ret = avcodec_receive_frame(m_codecCtx, m_frame);
                if (ret >= 0) {
                    VideoFrame frame;
                    frame.pts = m_frame->pts * av_q2d(m_formatCtx->streams[m_videoStreamIndex]->time_base);
                    
                    // Convert to RGB
                    return convertFrameToRGB(frame);
                }
            }
        }
        av_packet_unref(m_packet);
    }

    return VideoFrame();
}

bool VideoDecoder::getNextFrame(VideoFrame& outFrame) {
    if (!isOpen()) return false;

    while (av_read_frame(m_formatCtx, m_packet) >= 0) {
        if (m_packet->stream_index == m_videoStreamIndex) {
            int ret = avcodec_send_packet(m_codecCtx, m_packet);
            if (ret >= 0) {
                ret = avcodec_receive_frame(m_codecCtx, m_frame);
                if (ret >= 0) {
                    outFrame.pts = m_frame->pts * av_q2d(m_formatCtx->streams[m_videoStreamIndex]->time_base);
                    outFrame = convertFrameToRGB(outFrame);
                    av_packet_unref(m_packet);
                    return true;
                }
            }
        }
        av_packet_unref(m_packet);
    }

    return false;
}

QVector<VideoFrame> VideoDecoder::getFramesInRange(double startTime, double endTime, int targetFps) {
    QVector<VideoFrame> frames;
    
    // Seek to start
    int64_t startTs = startTime * AV_TIME_BASE;
    avformat_seek_file(m_formatCtx, -1, INT64_MIN, startTs, INT64_MAX, 0);
    avcodec_flush_buffers(m_codecCtx);
    m_currentTime = startTime;

    double frameInterval = 1.0 / targetFps;
    double nextFrameTime = startTime;

    while (m_currentTime < endTime) {
        VideoFrame frame;
        if (getNextFrame(frame)) {
            if (frame.pts >= nextFrameTime) {
                frames.append(frame.clone());
                nextFrameTime += frameInterval;
            }
        } else {
            break;
        }
    }

    return frames;
}

QImage VideoDecoder::generateThumbnail(double timeSeconds, int width, int height) {
    VideoFrame frame = getFrameAtTime(timeSeconds);
    if (frame.width == 0) return QImage();

    // Resize to thumbnail size
    SwsContext* thumbCtx = sws_getContext(
        frame.width, frame.height, AV_PIX_FMT_RGB24,
        width, height, AV_PIX_FMT_RGB32,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );

    QImage thumbnail(width, height, QImage::Format_RGB32);
    uint8_t* dest[4] = {thumbnail.bits(), nullptr, nullptr, nullptr};
    int destLinesize[4] = {static_cast<int>(thumbnail.bytesPerLine()), 0, 0, 0};

    sws_scale(thumbCtx, frame.data, frame.linesize, 0, frame.height, dest, destLinesize);
    sws_freeContext(thumbCtx);

    return thumbnail.copy();
}

QVector<QImage> VideoDecoder::generateThumbnails(int count, int width, int height) {
    QVector<QImage> thumbnails;
    if (!isOpen() || count <= 0) return thumbnails;

    double interval = m_duration / (count + 1);
    for (int i = 1; i <= count; i++) {
        double time = interval * i;
        QImage thumb = generateThumbnail(time, width, height);
        if (!thumb.isNull()) {
            thumbnails.append(thumb);
        }
    }

    return thumbnails;
}

VideoFrame VideoDecoder::convertFrameToRGB(const VideoFrame& srcFrame) {
    VideoFrame rgbFrame;
    rgbFrame.allocate(m_width, m_height, AV_PIX_FMT_RGB24);
    rgbFrame.pts = srcFrame.pts;

    if (!m_swsCtx) {
        m_swsCtx = sws_getContext(
            m_width, m_height, m_codecCtx->pix_fmt,
            m_width, m_height, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, nullptr, nullptr, nullptr
        );
    }

    sws_scale(m_swsCtx, m_frame->data, m_frame->linesize, 0, m_height,
              rgbFrame.data, rgbFrame.linesize);

    return rgbFrame;
}

} // namespace cinemastudio
