#include "Clip.h"
#include <QUuid>
#include <QFileInfo>

namespace cinemastudio {

Clip::Clip()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_type(ClipType::Video) {
}

Clip::Clip(const QString& filePath, ClipType type)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_filePath(filePath)
    , m_type(type)
    , m_name(QFileInfo(filePath).fileName()) {
}

Clip::~Clip() = default;

void Clip::setFilePath(const QString& path) {
    m_filePath = path;
    m_name = QFileInfo(path).fileName();
}

void Clip::setProperties(const ClipProperties& props) {
    m_properties = props;
}

void Clip::setThumbnail(const QImage& thumb) {
    m_thumbnail = thumb;
}

void Clip::setName(const QString& name) {
    m_name = name;
}

void Clip::setDimensions(int w, int h) {
    m_width = w;
    m_height = h;
}

void Clip::setDuration(double duration) {
    m_properties.duration = duration;
    m_properties.endTime = duration;
}

void Clip::setHasAudio(bool has) {
    m_hasAudio = has;
}

void Clip::setFps(double fps) {
    m_fps = fps;
}

void Clip::trimStart(double time) {
    if (time >= m_properties.endTime) return;
    m_properties.startTime = std::max(0.0, time);
    m_properties.duration = m_properties.endTime - m_properties.startTime;
}

void Clip::trimEnd(double time) {
    if (time <= m_properties.startTime) return;
    m_properties.endTime = std::min(m_properties.duration, time);
    m_properties.duration = m_properties.endTime - m_properties.startTime;
}

void Clip::split(double time, Clip& outClip) {
    outClip = *this;
    outClip.m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    
    // Original clip ends at split point
    m_properties.endTime = time;
    m_properties.duration = m_properties.endTime - m_properties.startTime;
    
    // New clip starts at split point
    outClip.m_properties.startTime = time;
    outClip.m_properties.timelinePosition = m_properties.timelinePosition + (time - m_properties.startTime);
    outClip.m_properties.duration = outClip.m_properties.endTime - outClip.m_properties.startTime;
}

QJsonObject Clip::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["filePath"] = m_filePath;
    json["name"] = m_name;
    json["type"] = static_cast<int>(m_type);
    json["startTime"] = m_properties.startTime;
    json["endTime"] = m_properties.endTime;
    json["duration"] = m_properties.duration;
    json["timelinePosition"] = m_properties.timelinePosition;
    json["trackIndex"] = m_properties.trackIndex;
    json["volume"] = m_properties.volume;
    json["speed"] = m_properties.speed;
    json["muted"] = m_properties.muted;
    json["frozen"] = m_properties.frozen;
    json["width"] = m_width;
    json["height"] = m_height;
    json["hasAudio"] = m_hasAudio;
    json["fps"] = m_fps;
    return json;
}

void Clip::fromJson(const QJsonObject& json) {
    m_id = json["id"].toString();
    m_filePath = json["filePath"].toString();
    m_name = json["name"].toString();
    m_type = static_cast<ClipType>(json["type"].toInt());
    m_properties.startTime = json["startTime"].toDouble();
    m_properties.endTime = json["endTime"].toDouble();
    m_properties.duration = json["duration"].toDouble();
    m_properties.timelinePosition = json["timelinePosition"].toDouble();
    m_properties.trackIndex = json["trackIndex"].toInt();
    m_properties.volume = json["volume"].toDouble();
    m_properties.speed = json["speed"].toDouble();
    m_properties.muted = json["muted"].toBool();
    m_properties.frozen = json["frozen"].toBool();
    m_width = json["width"].toInt();
    m_height = json["height"].toInt();
    m_hasAudio = json["hasAudio"].toBool();
    m_fps = json["fps"].toDouble();
}

std::unique_ptr<Clip> Clip::clone() const {
    auto clip = std::make_unique<Clip>();
    clip->m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    clip->m_filePath = m_filePath;
    clip->m_name = m_name;
    clip->m_type = m_type;
    clip->m_properties = m_properties;
    clip->m_thumbnail = m_thumbnail;
    clip->m_width = m_width;
    clip->m_height = m_height;
    clip->m_hasAudio = m_hasAudio;
    clip->m_fps = m_fps;
    return clip;
}

} // namespace cinemastudio
