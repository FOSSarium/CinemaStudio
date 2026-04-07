#pragma once

#include <QString>
#include <vector>
#include <functional>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}

namespace cinemastudio {

class AudioDecoder {
public:
    AudioDecoder();
    ~AudioDecoder();

    bool open(const QString& filePath);
    void close();
    bool isOpen() const { return m_formatCtx != nullptr; }

    // Get audio samples as float array (normalized -1.0 to 1.0)
    std::vector<float> getSamples(double startTime, double endTime);
    
    // Get waveform data (downsampled for visualization)
    std::vector<float> getWaveformData(int sampleCount = 1000);

    // Info
    int getSampleRate() const { return m_sampleRate; }
    int getChannels() const { return m_channels; }
    double getDuration() const { return m_duration; }
    int getAudioStreamIndex() const { return m_audioStreamIndex; }

private:
    bool initAudioStream();

    AVFormatContext* m_formatCtx = nullptr;
    AVCodecContext* m_codecCtx = nullptr;
    AVPacket* m_packet = nullptr;
    AVFrame* m_frame = nullptr;
    SwrContext* m_swrCtx = nullptr;
    
    int m_audioStreamIndex = -1;
    int m_sampleRate = 0;
    int m_channels = 0;
    double m_duration = 0.0;
    
    QString m_filePath;
};

} // namespace cinemastudio
