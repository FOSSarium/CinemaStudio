#include "TextOverlay.h"

namespace cinemastudio {

TextOverlay::TextOverlay() {
    m_font.setFamily("Arial");
    m_font.setPointSize(24);
    m_font.setBold(true);
}

TextOverlay::~TextOverlay() = default;

void TextOverlay::setText(const QString& text) {
    m_text = text;
}

void TextOverlay::setFont(const QFont& font) {
    m_font = font;
}

void TextOverlay::setTextColor(const QColor& color) {
    m_textColor = color;
}

void TextOverlay::setBackgroundColor(const QColor& color) {
    m_backgroundColor = color;
}

void TextOverlay::setHasBackground(bool has) {
    m_hasBackground = has;
}

void TextOverlay::setPosition(const QPointF& pos) {
    m_position = pos;
}

void TextOverlay::setSize(const QSizeF& size) {
    m_size = size;
}

void TextOverlay::setAlignment(TextAlignment alignment) {
    m_alignment = alignment;
}

void TextOverlay::setTimeRange(double start, double end) {
    m_startTime = std::max(0.0, start);
    m_endTime = std::max(start, end);
}

void TextOverlay::setOpacity(int opacity) {
    m_opacity = std::clamp(opacity, 0, 255);
}

void TextOverlay::setOutline(const QColor& color, int width) {
    m_hasOutline = true;
    m_outlineColor = color;
    m_outlineWidth = std::max(0, width);
}

QJsonObject TextOverlay::toJson() const {
    QJsonObject json;
    json["id"] = m_id;
    json["text"] = m_text;
    json["fontName"] = m_font.family();
    json["fontSize"] = m_font.pointSize();
    json["fontBold"] = m_font.bold();
    json["fontItalic"] = m_font.italic();
    json["textColor"] = m_textColor.name();
    json["backgroundColor"] = m_backgroundColor.name();
    json["hasBackground"] = m_hasBackground;
    json["positionX"] = m_position.x();
    json["positionY"] = m_position.y();
    json["sizeWidth"] = m_size.width();
    json["sizeHeight"] = m_size.height();
    json["alignment"] = static_cast<int>(m_alignment);
    json["startTime"] = m_startTime;
    json["endTime"] = m_endTime;
    json["opacity"] = m_opacity;
    json["hasOutline"] = m_hasOutline;
    json["outlineColor"] = m_outlineColor.name();
    json["outlineWidth"] = m_outlineWidth;
    return json;
}

void TextOverlay::fromJson(const QJsonObject& json) {
    m_id = json["id"].toString();
    m_text = json["text"].toString();
    m_font.setFamily(json["fontName"].toString());
    m_font.setPointSize(json["fontSize"].toInt());
    m_font.setBold(json["fontBold"].toBool());
    m_font.setItalic(json["fontItalic"].toBool());
    m_textColor = QColor(json["textColor"].toString());
    m_backgroundColor = QColor(json["backgroundColor"].toString());
    m_hasBackground = json["hasBackground"].toBool();
    m_position = QPointF(json["positionX"].toDouble(), json["positionY"].toDouble());
    m_size = QSizeF(json["sizeWidth"].toDouble(), json["sizeHeight"].toDouble());
    m_alignment = static_cast<TextAlignment>(json["alignment"].toInt());
    m_startTime = json["startTime"].toDouble();
    m_endTime = json["endTime"].toDouble();
    m_opacity = json["opacity"].toInt();
    m_hasOutline = json["hasOutline"].toBool();
    m_outlineColor = QColor(json["outlineColor"].toString());
    m_outlineWidth = json["outlineWidth"].toInt();
}

} // namespace cinemastudio
