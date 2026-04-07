#pragma once

#include <QString>
#include <vector>

namespace cinemastudio {

class WaveformGenerator {
public:
    WaveformGenerator();
    ~WaveformGenerator();

    // Generate waveform data from audio file
    std::vector<float> generateWaveform(const QString& audioPath, int sampleCount = 1000);
    
    // Generate waveform with RMS values
    struct WaveformData {
        std::vector<float> peaks;    // Peak values
        std::vector<float> rms;      // RMS values
        int sampleCount;
    };
    
    WaveformData generateWaveformDetailed(const QString& audioPath, int sampleCount = 1000);

private:
    std::vector<float> downsampleToWaveform(const std::vector<float>& samples, int targetCount);
    float calculateRMS(const std::vector<float>& samples);
};

} // namespace cinemastudio
