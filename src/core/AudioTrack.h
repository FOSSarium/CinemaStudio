#pragma once

#include <QString>
#include <QJsonObject>
#include <QUuid>
#include <vector>

namespace cinemastudio {

class AudioTrack {
public:
    AudioTrack();
    AudioTrack(const QString& filePath);
    ~AudioTrack();

    QString getId() const { return m_id; }
    QString getFilePath() const { return m_filePath; }
    QString getName() const { return m_name; }
    double getDuration() const { return m_duration; }
    float getVolume() const { return m_volume; }
    bool isMuted() const { return m_muted; }
    double getStartTime() const { return m_startTime; }
    int getSampleRate() const { return m_sampleRate; }
    int getChannels() const { return m_channels; }
    
    const std::vector<float>& getWaveformData() const { return m_waveformData; }

    void setFilePath(const QString& path);
    void setName(const QString& name);
    void setVolume(float volume);
    void setMuted(bool muted);
    void setStartTime(double time);
    void setSampleRate(int rate);
    void setChannels(int count);
    void setWaveformData(const std::vector<float>& data);

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    QString m_id;
    QString m_filePath;
    QString m_name;
    double m_duration = 0.0;
    float m_volume = 1.0f;
    bool m_muted = false;
    double m_startTime = 0.0;
    int m_sampleRate = 44100;
    int m_channels = 2;
    std::vector<float> m_waveformData;
};

} // namespace cinemastudio
