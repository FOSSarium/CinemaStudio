#include "AudioDecoder.h"
#include "FFmpegEngine.h"
#include <QDebug>
#include <algorithm>
#include <cmath>

namespace cinemastudio {

AudioDecoder::AudioDecoder() = default;
AudioDecoder::~AudioDecoder() { close(); }

bool AudioDecoder::open(const QString& filePath) {
    close();
    m_filePath = filePath;
    int ret = avformat_open_input(&m_formatCtx, filePath.toUtf8().constData(), nullptr, nullptr);
    if (ret < 0) { qCritical() << "Failed to open audio:" << FFmpegEngine::getErrorString(ret); return false; }
    ret = avformat_find_stream_info(m_formatCtx, nullptr);
    if (ret < 0) { close(); return false; }
    
    // Find audio stream
    for (unsigned int i = 0; i < m_formatCtx->nb_streams; i++) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            m_audioStreamIndex = i;
            break;
        }
    }
    if (m_audioStreamIndex < 0) { close(); return false; }
    
    AVCodecParameters* codecParams = m_formatCtx->streams[m_audioStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) { close(); return false; }
    
    m_codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(m_codecCtx, codecParams);
    ret = avcodec_open2(m_codecCtx, codec, nullptr);
    if (ret < 0) { close(); return false; }
    
    m_sampleRate = m_codecCtx->sample_rate;
    m_channels = m_codecCtx->channels;
    m_duration = m_formatCtx->duration / (double)AV_TIME_BASE;
    
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    return true;
}

void AudioDecoder::close() {
    if (m_swrCtx) swr_free(&m_swrCtx);
    if (m_codecCtx) avcodec_free_context(&m_codecCtx);
    if (m_formatCtx) avformat_close_input(&m_formatCtx);
    if (m_packet) av_packet_free(&m_packet);
    if (m_frame) av_frame_free(&m_frame);
    m_audioStreamIndex = -1; m_sampleRate = 0; m_channels = 0; m_duration = 0.0;
}

std::vector<float> AudioDecoder::getSamples(double startTime, double endTime) {
    std::vector<float> samples;
    if (!isOpen()) return samples;
    // Stub implementation - would decode audio samples here
    return samples;
}

std::vector<float> AudioDecoder::getWaveformData(int sampleCount) {
    return std::vector<float>(sampleCount, 0.0f);
}

} // namespace cinemastudio
