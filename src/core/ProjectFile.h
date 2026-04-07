#pragma once

#include "Track.h"
#include "Effect.h"
#include <vector>
#include <memory>
#include <QString>
#include <QJsonObject>
#include <QSize>
#include <functional>

namespace cinemastudio {

class ProjectFile {
public:
    ProjectFile();
    ~ProjectFile();

    // Project info
    QString getName() const { return m_name; }
    QString getFilePath() const { return m_filePath; }
    QSize getResolution() const { return m_resolution; }
    double getFps() const { return m_fps; }
    bool isModified() const { return m_modified; }

    void setName(const QString& name);
    void setFilePath(const QString& path);
    void setResolution(int width, int height);
    void setFps(double fps);
    void setModified(bool modified);

    // Tracks
    Track* addTrack(TrackType type, const QString& name = QString());
    void removeTrack(const QString& trackId);
    Track* getTrack(int index);
    const Track* getTrack(int index) const;
    int getTrackCount() const { return static_cast<int>(m_tracks.size()); }
    
    std::vector<Track*> getVideoTracks();
    std::vector<Track*> getAudioTracks();
    std::vector<Track*> getTextTracks();

    // Effects
    void addEffect(std::unique_ptr<Effect> effect);
    void removeEffect(const QString& effectId);
    Effect* getEffect(const QString& effectId);
    const std::vector<std::unique_ptr<Effect>>& getEffects() const { return m_effects; }

    // Timeline
    double getTimelineDuration() const;
    void setPlayheadPosition(double time);
    double getPlayheadPosition() const { return m_playheadPosition; }

    // Serialization
    bool saveToFile(const QString& filePath);
    bool loadFromFile(const QString& filePath);
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

    // Clear
    void clear();

    // Signals simulation
    using ChangeCallback = std::function<void()>;
    void setOnChange(ChangeCallback callback) { m_onChange = callback; }

private:
    void markModified();

    QString m_name = "Untitled Project";
    QString m_filePath;
    QSize m_resolution = QSize(1920, 1080);
    double m_fps = 30.0;
    bool m_modified = false;
    double m_playheadPosition = 0.0;

    std::vector<std::unique_ptr<Track>> m_tracks;
    std::vector<std::unique_ptr<Effect>> m_effects;

    ChangeCallback m_onChange;
};

} // namespace cinemastudio
