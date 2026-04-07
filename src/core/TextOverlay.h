#pragma once

#include <QString>
#include <QColor>
#include <QFont>
#include <QJsonObject>
#include <QPointF>
#include <QSizeF>
#include <QUuid>

namespace cinemastudio {

enum class TextAlignment {
    Left,
    Center,
    Right
};

class TextOverlay {
public:
    TextOverlay();
    ~TextOverlay();

    // Getters
    QString getId() const { return m_id; }
    QString getText() const { return m_text; }
    QFont getFont() const { return m_font; }
    QColor getTextColor() const { return m_textColor; }
    QColor getBackgroundColor() const { return m_backgroundColor; }
    bool hasBackground() const { return m_hasBackground; }
    QPointF getPosition() const { return m_position; }
    QSizeF getSize() const { return m_size; }
    TextAlignment getAlignment() const { return m_alignment; }
    double getStartTime() const { return m_startTime; }
    double getEndTime() const { return m_endTime; }
    int getOpacity() const { return m_opacity; }
    bool hasOutline() const { return m_hasOutline; }
    QColor getOutlineColor() const { return m_outlineColor; }
    int getOutlineWidth() const { return m_outlineWidth; }

    // Setters
    void setText(const QString& text);
    void setFont(const QFont& font);
    void setTextColor(const QColor& color);
    void setBackgroundColor(const QColor& color);
    void setHasBackground(bool has);
    void setPosition(const QPointF& pos);
    void setSize(const QSizeF& size);
    void setAlignment(TextAlignment alignment);
    void setTimeRange(double start, double end);
    void setOpacity(int opacity);
    void setOutline(const QColor& color, int width);

    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    QString m_id;
    QString m_text = "Text";
    QFont m_font;
    QColor m_textColor = Qt::white;
    QColor m_backgroundColor = Qt::black;
    bool m_hasBackground = false;
    QPointF m_position = QPointF(0.5, 0.5); // Normalized 0-1
    QSizeF m_size = QSizeF(0.5, 0.1);      // Normalized 0-1
    TextAlignment m_alignment = TextAlignment::Center;
    double m_startTime = 0.0;
    double m_endTime = 5.0;
    int m_opacity = 255;
    bool m_hasOutline = false;
    QColor m_outlineColor = Qt::black;
    int m_outlineWidth = 2;
};

} // namespace cinemastudio
