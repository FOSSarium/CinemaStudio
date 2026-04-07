#include "AudioTrack.h"
#include <QFileInfo>

namespace cinemastudio {

AudioTrack::AudioTrack()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces)) {
}

AudioTrack::AudioTrack(const QString& filePath)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_filePath(filePath)
    , m_name(QFileInfo(filePath).fileName()) {
}

AudioTrack::~AudioTrack() = default;

void AudioTrack::setFilePath(const QString& path) {
    m_filePath = path;
    m_name = QFileInfo(path).fileName();
}

void AudioTrack::setName(const QString& name) {
    m_name = name;
}

void AudioTrack::setVolume(float volume) {
    m_volume = std::clamp(volume, 0.0f, 2.0f);
}

void AudioTrack::setMuted(bool muted) {
    m_muted = muted;
}

void AudioTrack::setStartTime(double time) {
    m_startTime = std::max(0.0, time);
}

void AudioTrack::setSampleRate(int rate) {
    m_sampleRate = rate;
}

void AudioTrack::setChannels(int count) {
    m_channels = count;
}

void AudioTrack::setWaveformData(const std::vector<float>& data) {
    m_waveformData = data;
}

QJsonObject AudioTrack::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["filePath"] = m_filePath;
    json["name"] = m_name;
    json["duration"] = m_duration;
    json["volume"] = m_volume;
    json["muted"] = m_muted;
    json["startTime"] = m_startTime;
    json["sampleRate"] = m_sampleRate;
    json["channels"] = m_channels;
    return json;
}

void AudioTrack::fromJson(const QJsonObject& json) {
    m_id = json["id"].toString();
    m_filePath = json["filePath"].toString();
    m_name = json["name"].toString();
    m_duration = json["duration"].toDouble();
    m_volume = json["volume"].toDouble();
    m_muted = json["muted"].toBool();
    m_startTime = json["startTime"].toDouble();
    m_sampleRate = json["sampleRate"].toInt();
    m_channels = json["channels"].toInt();
}

} // namespace cinemastudio
