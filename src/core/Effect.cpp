#include "Effect.h"

namespace cinemastudio {

QString Effect::effectTypeToString(EffectType type) {
  switch (type) {
  case EffectType::Blur:
    return "Blur";
  case EffectType::Sharpen:
    return "Sharpen";
  case EffectType::ColorCorrection:
    return "Color Correction";
  case EffectType::BrightnessContrast:
    return "Brightness/Contrast";
  case EffectType::HueSaturation:
    return "Hue/Saturation";
  case EffectType::ChromaKey:
    return "Chroma Key";
  case EffectType::Vignette:
    return "Vignette";
  case EffectType::Sepia:
    return "Sepia";
  case EffectType::Grayscale:
    return "Grayscale";
  case EffectType::Invert:
    return "Invert";
  case EffectType::FadeIn:
    return "Fade In";
  case EffectType::FadeOut:
    return "Fade Out";
  case EffectType::Crossfade:
    return "Crossfade";
  case EffectType::Wipe:
    return "Wipe";
  case EffectType::Slide:
    return "Slide";
  case EffectType::Zoom:
    return "Zoom";
  case EffectType::AudioFadeIn:
    return "Audio Fade In";
  case EffectType::AudioFadeOut:
    return "Audio Fade Out";
  case EffectType::AudioNormalize:
    return "Audio Normalize";
  case EffectType::AudioEqualizer:
    return "Audio Equalizer";
  case EffectType::AudioReverb:
    return "Audio Reverb";
  }
  return "Unknown";
}

Effect::Effect()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces)),
      m_type(EffectType::Blur) {}

Effect::~Effect() = default;

Effect::Effect(EffectType type, const QString &name)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces)), m_type(type),
      m_name(name.isEmpty() ? effectTypeToString(type) : name) {}

void Effect::setName(const QString &name) { m_name = name; }

void Effect::setEnabled(bool enabled) { m_enabled = enabled; }

void Effect::setTimeRange(double start, double end) {
  m_startTime = std::max(0.0, start);
  m_endTime = std::max(start, end);
}

void Effect::setIntensity(float intensity) {
  m_intensity = std::clamp(intensity, 0.0f, 100.0f);
}

void Effect::setParameter(const QString &key, const QVariant &value) {
  m_parameters[key] = value;
}


Effect Effect::createBlur(float intensity) {
  Effect effect(EffectType::Blur, "Blur");
  effect.setIntensity(intensity);
  effect.setParameter("radius", intensity);
  return effect;
}

Effect Effect::createSharpen(float intensity) {
  Effect effect(EffectType::Sharpen, "Sharpen");
  effect.setIntensity(intensity);
  effect.setParameter("amount", intensity);
  return effect;
}

Effect Effect::createColorCorrection(float brightness, float contrast,
                                     float saturation) {
  Effect effect(EffectType::ColorCorrection, "Color Correction");
  effect.setParameter("brightness", brightness);
  effect.setParameter("contrast", contrast);
  effect.setParameter("saturation", saturation);
  return effect;
}

Effect Effect::createVignette(float intensity) {
  Effect effect(EffectType::Vignette, "Vignette");
  effect.setIntensity(intensity);
  effect.setParameter("softness", 0.5f);
  return effect;
}

Effect Effect::createSepia() {
  Effect effect(EffectType::Sepia, "Sepia");
  effect.setParameter("intensity", 1.0f);
  return effect;
}

Effect Effect::createFadeIn(double duration) {
  Effect effect(EffectType::FadeIn, "Fade In");
  effect.setTimeRange(0.0, duration);
  return effect;
}

Effect Effect::createFadeOut(double duration) {
  Effect effect(EffectType::FadeOut, "Fade Out");
  effect.setTimeRange(0.0, duration);
  return effect;
}

Effect Effect::createCrossfade(double duration) {
  Effect effect(EffectType::Crossfade, "Crossfade");
  effect.setTimeRange(0.0, duration);
  return effect;
}

QJsonObject Effect::toJson() const {
  QJsonObject json;
  json["id"] = m_id;
  json["name"] = m_name;
  json["type"] = static_cast<int>(m_type);
  json["enabled"] = m_enabled;
  json["startTime"] = m_startTime;
  json["endTime"] = m_endTime;
  json["intensity"] = m_intensity;

  QJsonObject params;
  for (auto it = m_parameters.begin(); it != m_parameters.end(); ++it) {
    params[it.key()] = QJsonValue::fromVariant(it.value());
  }
  json["parameters"] = params;

  return json;
}

void Effect::fromJson(const QJsonObject &json) {
  m_id = json["id"].toString();
  m_name = json["name"].toString();
  m_type = static_cast<EffectType>(json["type"].toInt());
  m_enabled = json["enabled"].toBool();
  m_startTime = json["startTime"].toDouble();
  m_endTime = json["endTime"].toDouble();
  m_intensity = json["intensity"].toDouble();

  m_parameters.clear();
  QJsonObject params = json["parameters"].toObject();
  for (auto it = params.begin(); it != params.end(); ++it) {
    m_parameters[it.key()] = it.value().toVariant();
  }
}

} // namespace cinemastudio
