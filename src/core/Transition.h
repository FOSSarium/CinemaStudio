#pragma once

#include <QString>
#include <QJsonObject>
#include <QUuid>

namespace cinemastudio {

enum class TransitionType {
    None,
    Fade,
    Crossfade,
    WipeLeft,
    WipeRight,
    WipeUp,
    WipeDown,
    SlideLeft,
    SlideRight,
    SlideUp,
    SlideDown,
    ZoomIn,
    ZoomOut,
    CircleOpen,
    CircleClose,
    Dissolve
};

class Transition {
public:
    Transition();
    Transition(TransitionType type, double duration);
    ~Transition();

    QString getId() const { return m_id; }
    QString getName() const { return m_name; }
    TransitionType getType() const { return m_type; }
    double getDuration() const { return m_duration; }
    QString getClipIdBefore() const { return m_clipIdBefore; }
    QString getClipIdAfter() const { return m_clipIdAfter; }

    void setType(TransitionType type);
    void setDuration(double duration);
    void setClipIds(const QString& before, const QString& after);

    static QString typeToString(TransitionType type);

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    QString m_id;
    QString m_name;
    TransitionType m_type;
    double m_duration;
    QString m_clipIdBefore;
    QString m_clipIdAfter;
};

} // namespace cinemastudio
