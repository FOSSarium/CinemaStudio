#include "Transition.h"

namespace cinemastudio {

Transition::Transition()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_type(TransitionType::None)
    , m_duration(1.0) {
}

Transition::Transition(TransitionType type, double duration)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_type(type)
    , m_duration(duration)
    , m_name(typeToString(type)) {
}

Transition::~Transition() = default;

void Transition::setType(TransitionType type) {
    m_type = type;
    m_name = typeToString(type);
}

void Transition::setDuration(double duration) {
    m_duration = std::max(0.0, duration);
}

void Transition::setClipIds(const QString& before, const QString& after) {
    m_clipIdBefore = before;
    m_clipIdAfter = after;
}

QString Transition::typeToString(TransitionType type) {
    switch (type) {
        case TransitionType::None: return "None";
        case TransitionType::Fade: return "Fade";
        case TransitionType::Crossfade: return "Crossfade";
        case TransitionType::WipeLeft: return "Wipe Left";
        case TransitionType::WipeRight: return "Wipe Right";
        case TransitionType::WipeUp: return "Wipe Up";
        case TransitionType::WipeDown: return "Wipe Down";
        case TransitionType::SlideLeft: return "Slide Left";
        case TransitionType::SlideRight: return "Slide Right";
        case TransitionType::SlideUp: return "Slide Up";
        case TransitionType::SlideDown: return "Slide Down";
        case TransitionType::ZoomIn: return "Zoom In";
        case TransitionType::ZoomOut: return "Zoom Out";
        case TransitionType::CircleOpen: return "Circle Open";
        case TransitionType::CircleClose: return "Circle Close";
        case TransitionType::Dissolve: return "Dissolve";
    }
    return "Unknown";
}

QJsonObject Transition::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["name"] = m_name;
    json["type"] = static_cast<int>(m_type);
    json["duration"] = m_duration;
    json["clipIdBefore"] = m_clipIdBefore;
    json["clipIdAfter"] = m_clipIdAfter;
    return json;
}

void Transition::fromJson(const QJsonObject& json) {
    m_id = json["id"].toString();
    m_name = json["name"].toString();
    m_type = static_cast<TransitionType>(json["type"].toInt());
    m_duration = json["duration"].toDouble();
    m_clipIdBefore = json["clipIdBefore"].toString();
    m_clipIdAfter = json["clipIdAfter"].toString();
}

} // namespace cinemastudio
