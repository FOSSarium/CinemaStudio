#pragma once

#include <QImage>
#include <QJsonObject>
#include <QString>
#include <memory>

namespace cinemastudio {

enum class ClipType { Video, Audio, Image, Text, Transition };

struct ClipProperties {
  double startTime = 0.0;
  double endTime = 0.0;
  double duration = 0.0;
  double timelinePosition = 0.0;
  int trackIndex = 0;
  float volume = 1.0f;
  float speed = 1.0f;
  bool muted = false;
  bool frozen = false;
};

class Clip {
public:
  Clip();
  Clip(const QString &filePath, ClipType type);
  ~Clip();

  // Getters
  QString getId() const { return m_id; }
  QString getFilePath() const { return m_filePath; }
  ClipType getType() const { return m_type; }
  const ClipProperties &getProperties() const { return m_properties; }
  ClipProperties &getProperties() { return m_properties; }
  QString getName() const { return m_name; }
  QImage getThumbnail() const { return m_thumbnail; }
  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
  double getDuration() const { return m_properties.duration; }
  bool hasAudio() const { return m_hasAudio; }
  double getFps() const { return m_fps; }

  // Setters
  void setFilePath(const QString &path);
  void setProperties(const ClipProperties &props);
  void setThumbnail(const QImage &thumb);
  void setName(const QString &name);
  void setDimensions(int w, int h);
  void setDuration(double duration);
  void setHasAudio(bool has);
  void setFps(double fps);

  // Trim
  void trimStart(double time);
  void trimEnd(double time);
  void split(double time, Clip &outClip);

  // Serialization
  QJsonObject toJson() const;
  void fromJson(const QJsonObject &json);

  // Clone
  std::unique_ptr<Clip> clone() const;

private:
  QString m_id;
  QString m_filePath;
  QString m_name;
  ClipType m_type;
  ClipProperties m_properties;
  QImage m_thumbnail;
  int m_width = 0;
  int m_height = 0;
  bool m_hasAudio = false;
  double m_fps = 0.0;
};

} // namespace cinemastudio
