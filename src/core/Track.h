#pragma once

#include "Clip.h"
#include <vector>
#include <memory>
#include <QJsonArray>
#include <QString>
#include <QJsonObject>
#include <QColor>

namespace cinemastudio {

enum class TrackType {
    Video,
    Audio,
    Text
};

class Track {
public:
    Track();
    Track(TrackType type, const QString& name = QString());
    ~Track();

    // Getters
    QString getId() const { return m_id; }
    QString getName() const { return m_name; }
    TrackType getType() const { return m_type; }
    bool isVisible() const { return m_visible; }
    bool isLocked() const { return m_locked; }
    QColor getColor() const { return m_color; }
    float getVolume() const { return m_volume; }
    bool isMuted() const { return m_muted; }
    
    const std::vector<std::unique_ptr<Clip>>& getClips() const { return m_clips; }
    std::vector<std::unique_ptr<Clip>>& getClips() { return m_clips; }

    // Setters
    void setName(const QString& name);
    void setVisible(bool visible);
    void setLocked(bool locked);
    void setColor(const QColor& color);
    void setVolume(float volume);
    void setMuted(bool muted);

    // Clip operations
    void addClip(std::unique_ptr<Clip> clip);
    void removeClip(const QString& clipId);
    Clip* getClipAt(int index);
    const Clip* getClipAt(int index) const;
    int getClipCount() const { return static_cast<int>(m_clips.size()); }
    
    Clip* getClipAtTime(double time);
    std::vector<Clip*> getClipsInRange(double start, double end);

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    QString m_id;
    QString m_name;
    TrackType m_type;
    bool m_visible = true;
    bool m_locked = false;
    QColor m_color;
    float m_volume = 1.0f;
    bool m_muted = false;
    std::vector<std::unique_ptr<Clip>> m_clips;
};

} // namespace cinemastudio
