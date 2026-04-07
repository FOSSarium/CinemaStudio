#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QUuid>
#include <QVariantMap>

namespace cinemastudio {

enum class EffectType {
  Blur,
  Sharpen,
  ColorCorrection,
  BrightnessContrast,
  HueSaturation,
  ChromaKey,
  Vignette,
  Sepia,
  Grayscale,
  Invert,
  FadeIn,
  FadeOut,
  Crossfade,
  Wipe,
  Slide,
  Zoom,
  AudioFadeIn,
  AudioFadeOut,
  AudioNormalize,
  AudioEqualizer,
  AudioReverb
};

class Effect {
public:
  Effect();
  Effect(EffectType type, const QString &name = QString());
  ~Effect();

  QString getId() const { return m_id; }
  QString getName() const { return m_name; }
  EffectType getType() const { return m_type; }
  bool isEnabled() const { return m_enabled; }
  double getStartTime() const { return m_startTime; }
  double getEndTime() const { return m_endTime; }
  float getIntensity() const { return m_intensity; }
  QVariantMap getParameters() const { return m_parameters; }

  void setName(const QString &name);
  void setEnabled(bool enabled);
  void setTimeRange(double start, double end);
  void setIntensity(float intensity);
  void setParameter(const QString &key, const QVariant &value);

  static Effect createBlur(float intensity = 5.0f);
  static Effect createSharpen(float intensity = 50.0f);
  static Effect createColorCorrection(float brightness = 0,
                                      float contrast = 1.0f,
                                      float saturation = 1.0f);
  static Effect createVignette(float intensity = 0.5f);
  static Effect createSepia();
  static Effect createFadeIn(double duration = 1.0);
  static Effect createFadeOut(double duration = 1.0);
  static Effect createCrossfade(double duration = 1.0);

  QJsonObject toJson() const;
  void fromJson(const QJsonObject &json);

  static QString effectTypeToString(EffectType type);

private:
  QString m_id;
  QString m_name;
  EffectType m_type;
  bool m_enabled = true;
  double m_startTime = 0.0;
  double m_endTime = 0.0;
  float m_intensity = 1.0f;
  QVariantMap m_parameters;
};

} // namespace cinemastudio
