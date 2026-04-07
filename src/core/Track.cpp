#include "Track.h"
#include <QUuid>
#include <algorithm>

namespace cinemastudio {

Track::Track()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces)),
      m_type(TrackType::Video), m_color(Qt::blue) {}

Track::Track(TrackType type, const QString &name)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces)), m_type(type),
      m_name(name.isEmpty() ? (type == TrackType::Video   ? "Video"
                               : type == TrackType::Audio ? "Audio"
                                                          : "Text")
                            : name) {

  switch (type) {
  case TrackType::Video:
    m_color = QColor(52, 152, 219);
    break;
  case TrackType::Audio:
    m_color = QColor(46, 204, 113);
    break;
  case TrackType::Text:
    m_color = QColor(155, 89, 182);
    break;
  }
}

Track::~Track() = default;

void Track::setName(const QString &name) { m_name = name; }

void Track::setVisible(bool visible) { m_visible = visible; }

void Track::setLocked(bool locked) { m_locked = locked; }

void Track::setColor(const QColor &color) { m_color = color; }

void Track::setVolume(float volume) {
  m_volume = std::clamp(volume, 0.0f, 2.0f);
}

void Track::setMuted(bool muted) { m_muted = muted; }

void Track::addClip(std::unique_ptr<Clip> clip) {
  clip->getProperties().trackIndex = static_cast<int>(m_clips.size());
  m_clips.push_back(std::move(clip));
}

void Track::removeClip(const QString &clipId) {
  m_clips.erase(std::remove_if(m_clips.begin(), m_clips.end(),
                               [&clipId](const std::unique_ptr<Clip> &clip) {
                                 return clip->getId() == clipId;
                               }),
                m_clips.end());
}

Clip *Track::getClipAt(int index) {
  if (index >= 0 && index < static_cast<int>(m_clips.size())) {
    return m_clips[index].get();
  }
  return nullptr;
}

const Clip *Track::getClipAt(int index) const {
  if (index >= 0 && index < static_cast<int>(m_clips.size())) {
    return m_clips[index].get();
  }
  return nullptr;
}

Clip *Track::getClipAtTime(double time) {
  for (auto &clip : m_clips) {
    double clipStart = clip->getProperties().timelinePosition;
    double clipEnd = clipStart + clip->getProperties().duration;
    if (time >= clipStart && time <= clipEnd) {
      return clip.get();
    }
  }
  return nullptr;
}

std::vector<Clip *> Track::getClipsInRange(double start, double end) {
  std::vector<Clip *> result;
  for (auto &clip : m_clips) {
    double clipStart = clip->getProperties().timelinePosition;
    double clipEnd = clipStart + clip->getProperties().duration;
    if (clipStart <= end && clipEnd >= start) {
      result.push_back(clip.get());
    }
  }
  return result;
}

QJsonObject Track::toJson() const {
  QJsonObject json;
  json["id"] = m_id;
  json["name"] = m_name;
  json["type"] = static_cast<int>(m_type);
  json["visible"] = m_visible;
  json["locked"] = m_locked;
  json["color"] = m_color.name();
  json["volume"] = m_volume;
  json["muted"] = m_muted;

  QJsonArray clipsArray;
  for (const auto &clip : m_clips) {
    clipsArray.append(clip->toJson());
  }
  json["clips"] = clipsArray;

  return json;
}

void Track::fromJson(const QJsonObject &json) {
  m_id = json["id"].toString();
  m_name = json["name"].toString();
  m_type = static_cast<TrackType>(json["type"].toInt());
  m_visible = json["visible"].toBool();
  m_locked = json["locked"].toBool();
  m_color = QColor(json["color"].toString());
  m_volume = json["volume"].toDouble();
  m_muted = json["muted"].toBool();

  m_clips.clear();
  QJsonArray clipsArray = json["clips"].toArray();
  for (const auto &clipJson : clipsArray) {
    auto clip = std::make_unique<Clip>();
    clip->fromJson(clipJson.toObject());
    m_clips.push_back(std::move(clip));
  }
}

} // namespace cinemastudio
