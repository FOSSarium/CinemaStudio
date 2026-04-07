#include "ProjectFile.h"
#include <QUuid>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <algorithm>

namespace cinemastudio {

ProjectFile::ProjectFile() = default;
ProjectFile::~ProjectFile() = default;

void ProjectFile::setName(const QString& name) {
    m_name = name;
    markModified();
}

void ProjectFile::setFilePath(const QString& path) {
    m_filePath = path;
}

void ProjectFile::setResolution(int width, int height) {
    m_resolution = QSize(width, height);
    markModified();
}

void ProjectFile::setFps(double fps) {
    m_fps = fps;
    markModified();
}

void ProjectFile::setModified(bool modified) {
    m_modified = modified;
}

Track* ProjectFile::addTrack(TrackType type, const QString& name) {
    auto track = std::make_unique<Track>(type, name);
    Track* ptr = track.get();
    m_tracks.push_back(std::move(track));
    markModified();
    return ptr;
}

void ProjectFile::removeTrack(const QString& trackId) {
    m_tracks.erase(
        std::remove_if(m_tracks.begin(), m_tracks.end(),
            [&trackId](const std::unique_ptr<Track>& track) {
                return track->getId() == trackId;
            }),
        m_tracks.end()
    );
    markModified();
}

Track* ProjectFile::getTrack(int index) {
    if (index >= 0 && index < static_cast<int>(m_tracks.size())) {
        return m_tracks[index].get();
    }
    return nullptr;
}

const Track* ProjectFile::getTrack(int index) const {
    if (index >= 0 && index < static_cast<int>(m_tracks.size())) {
        return m_tracks[index].get();
    }
    return nullptr;
}

std::vector<Track*> ProjectFile::getVideoTracks() {
    std::vector<Track*> result;
    for (auto& track : m_tracks) {
        if (track->getType() == TrackType::Video) {
            result.push_back(track.get());
        }
    }
    return result;
}

std::vector<Track*> ProjectFile::getAudioTracks() {
    std::vector<Track*> result;
    for (auto& track : m_tracks) {
        if (track->getType() == TrackType::Audio) {
            result.push_back(track.get());
        }
    }
    return result;
}

std::vector<Track*> ProjectFile::getTextTracks() {
    std::vector<Track*> result;
    for (auto& track : m_tracks) {
        if (track->getType() == TrackType::Text) {
            result.push_back(track.get());
        }
    }
    return result;
}

void ProjectFile::addEffect(std::unique_ptr<Effect> effect) {
    m_effects.push_back(std::move(effect));
    markModified();
}

void ProjectFile::removeEffect(const QString& effectId) {
    m_effects.erase(
        std::remove_if(m_effects.begin(), m_effects.end(),
            [&effectId](const std::unique_ptr<Effect>& effect) {
                return effect->getId() == effectId;
            }),
        m_effects.end()
    );
    markModified();
}

Effect* ProjectFile::getEffect(const QString& effectId) {
    for (auto& effect : m_effects) {
        if (effect->getId() == effectId) {
            return effect.get();
        }
    }
    return nullptr;
}

double ProjectFile::getTimelineDuration() const {
    double maxDuration = 0.0;
    for (const auto& track : m_tracks) {
        for (const auto& clip : track->getClips()) {
            double clipEnd = clip->getProperties().timelinePosition + clip->getProperties().duration;
            maxDuration = std::max(maxDuration, clipEnd);
        }
    }
    return maxDuration;
}

void ProjectFile::setPlayheadPosition(double time) {
    m_playheadPosition = std::max(0.0, time);
}

bool ProjectFile::saveToFile(const QString& filePath) {
    QJsonObject json = toJson();
    QJsonDocument doc(json);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    m_filePath = filePath;
    m_modified = false;
    return true;
}

bool ProjectFile::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        return false;
    }
    
    fromJson(doc.object());
    m_filePath = filePath;
    m_modified = false;
    return true;
}

QJsonObject ProjectFile::toJson() const {
    QJsonObject json;
    json["name"] = m_name;
    json["resolution"] = QJsonObject{
        {"width", m_resolution.width()},
        {"height", m_resolution.height()}
    };
    json["fps"] = m_fps;
    
    QJsonArray tracksArray;
    for (const auto& track : m_tracks) {
        tracksArray.append(track->toJson());
    }
    json["tracks"] = tracksArray;
    
    QJsonArray effectsArray;
    for (const auto& effect : m_effects) {
        effectsArray.append(effect->toJson());
    }
    json["effects"] = effectsArray;
    
    return json;
}

void ProjectFile::fromJson(const QJsonObject& json) {
    clear();
    
    m_name = json["name"].toString();
    
    QJsonObject res = json["resolution"].toObject();
    m_resolution = QSize(res["width"].toInt(), res["height"].toInt());
    m_fps = json["fps"].toDouble();
    
    QJsonArray tracksArray = json["tracks"].toArray();
    for (const auto& trackJson : tracksArray) {
        auto track = std::make_unique<Track>();
        track->fromJson(trackJson.toObject());
        m_tracks.push_back(std::move(track));
    }
    
    QJsonArray effectsArray = json["effects"].toArray();
    for (const auto& effectJson : effectsArray) {
        auto effect = std::make_unique<Effect>();
        effect->fromJson(effectJson.toObject());
        m_effects.push_back(std::move(effect));
    }
}

void ProjectFile::clear() {
    m_tracks.clear();
    m_effects.clear();
    m_playheadPosition = 0.0;
    markModified();
}

void ProjectFile::markModified() {
    m_modified = true;
    if (m_onChange) {
        m_onChange();
    }
}

} // namespace cinemastudio
