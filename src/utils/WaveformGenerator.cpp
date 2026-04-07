#include "WaveformGenerator.h"
#include "ffmpeg/AudioDecoder.h"
#include <cmath>
#include <algorithm>

namespace cinemastudio {

WaveformGenerator::WaveformGenerator() = default;
WaveformGenerator::~WaveformGenerator() = default;

std::vector<float> WaveformGenerator::generateWaveform(const QString& audioPath, int sampleCount) {
    AudioDecoder decoder;
    if (!decoder.open(audioPath)) {
        return std::vector<float>();
    }

    return decoder.getWaveformData(sampleCount);
}

WaveformGenerator::WaveformData WaveformGenerator::generateWaveformDetailed(const QString& audioPath, int sampleCount) {
    AudioDecoder decoder;
    if (!decoder.open(audioPath)) {
        return WaveformData{};
    }

    // Get all samples
    double duration = decoder.getDuration();
    std::vector<float> samples = decoder.getSamples(0, duration);
    
    if (samples.empty()) {
        return WaveformData{};
    }

    WaveformData data;
    data.sampleCount = sampleCount;
    data.peaks.reserve(sampleCount);
    data.rms.reserve(sampleCount);

    int samplesPerBlock = std::max(1, static_cast<int>(samples.size()) / sampleCount);

    for (int i = 0; i < sampleCount; i++) {
        int startIdx = i * samplesPerBlock;
        int endIdx = std::min(startIdx + samplesPerBlock, static_cast<int>(samples.size()));

        float maxVal = 0.0f;
        float sumSquares = 0.0f;

        for (int j = startIdx; j < endIdx; j++) {
            float sample = std::abs(samples[j]);
            maxVal = std::max(maxVal, sample);
            sumSquares += samples[j] * samples[j];
        }

        data.peaks.push_back(maxVal);
        data.rms.push_back(std::sqrt(sumSquares / (endIdx - startIdx)));
    }

    return data;
}

std::vector<float> WaveformGenerator::downsampleToWaveform(const std::vector<float>& samples, int targetCount) {
    std::vector<float> waveform;
    waveform.reserve(targetCount);

    int samplesPerBlock = std::max(1, static_cast<int>(samples.size()) / targetCount);

    for (int i = 0; i < targetCount; i++) {
        int startIdx = i * samplesPerBlock;
        int endIdx = std::min(startIdx + samplesPerBlock, static_cast<int>(samples.size()));

        float maxVal = 0.0f;
        for (int j = startIdx; j < endIdx; j++) {
            maxVal = std::max(maxVal, std::abs(samples[j]));
        }

        waveform.push_back(maxVal);
    }

    return waveform;
}

float WaveformGenerator::calculateRMS(const std::vector<float>& samples) {
    if (samples.empty()) return 0.0f;

    float sumSquares = 0.0f;
    for (float sample : samples) {
        sumSquares += sample * sample;
    }

    return std::sqrt(sumSquares / samples.size());
}

} // namespace cinemastudio
