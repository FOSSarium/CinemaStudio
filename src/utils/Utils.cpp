#include "Utils.h"
#include <QDir>
#include <QFileInfo>
#include <algorithm>
#include <cmath>

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace cinemastudio {

QString Utils::formatTime(double seconds) {
  if (seconds < 0)
    seconds = 0;
  int hrs = static_cast<int>(seconds) / 3600;
  int mins = (static_cast<int>(seconds) % 3600) / 60;
  int secs = static_cast<int>(seconds) % 60;
  int ms = static_cast<int>((seconds - static_cast<int>(seconds)) * 1000);

  if (hrs > 0) {
    return QString("%1:%2:%3.%4")
        .arg(hrs, 2, 10, QChar('0'))
        .arg(mins, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'))
        .arg(ms, 3, 10, QChar('0'));
  }
  return QString("%1:%2.%3")
      .arg(mins, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'))
      .arg(ms, 3, 10, QChar('0'));
}

QString Utils::formatTimeShort(double seconds) {
  if (seconds < 0)
    seconds = 0;
  int mins = static_cast<int>(seconds) / 60;
  int secs = static_cast<int>(seconds) % 60;
  return QString("%1:%2")
      .arg(mins, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));
}

double Utils::parseTime(const QString &timeStr) {
  QStringList parts = timeStr.split(':');
  if (parts.size() == 3) {
    return parts[0].toDouble() * 3600 + parts[1].toDouble() * 60 +
           parts[2].toDouble();
  } else if (parts.size() == 2) {
    return parts[0].toDouble() * 60 + parts[1].toDouble();
  }
  return timeStr.toDouble();
}

QString Utils::formatFileSize(qint64 bytes) {
  const QStringList units = {"B", "KB", "MB", "GB", "TB"};
  int unitIndex = 0;
  double size = bytes;

  while (size >= 1024 && unitIndex < units.size() - 1) {
    size /= 1024;
    unitIndex++;
  }

  return QString("%1 %2").arg(size, 0, 'f', 1).arg(units[unitIndex]);
}

QString Utils::getFileNameWithoutExt(const QString &filePath) {
  return QFileInfo(filePath).completeBaseName();
}

QString Utils::getFileExtension(const QString &filePath) {
  return QFileInfo(filePath).suffix().toLower();
}

QColor Utils::blendColors(const QColor &c1, const QColor &c2, float t) {
  t = std::clamp(t, 0.0f, 1.0f);
  int r = static_cast<int>(c1.red() * (1 - t) + c2.red() * t);
  int g = static_cast<int>(c1.green() * (1 - t) + c2.green() * t);
  int b = static_cast<int>(c1.blue() * (1 - t) + c2.blue() * t);
  int a = static_cast<int>(c1.alpha() * (1 - t) + c2.alpha() * t);
  return QColor(r, g, b, a);
}

QColor Utils::adjustBrightness(const QColor &color, float factor) {
  int r = std::clamp(static_cast<int>(color.red() * factor), 0, 255);
  int g = std::clamp(static_cast<int>(color.green() * factor), 0, 255);
  int b = std::clamp(static_cast<int>(color.blue() * factor), 0, 255);
  return QColor(r, g, b, color.alpha());
}

QColor Utils::adjustSaturation(const QColor &color, float factor) {
  int gray = static_cast<int>(0.299 * color.red() + 0.587 * color.green() +
                              0.114 * color.blue());
  int r = std::clamp(gray + static_cast<int>((color.red() - gray) * factor), 0,
                     255);
  int g = std::clamp(gray + static_cast<int>((color.green() - gray) * factor),
                     0, 255);
  int b = std::clamp(gray + static_cast<int>((color.blue() - gray) * factor), 0,
                     255);
  return QColor(r, g, b, color.alpha());
}

double Utils::clamp(double value, double min, double max) {
  return std::max(min, std::min(max, value));
}

float Utils::clamp(float value, float min, float max) {
  return std::max(min, std::min(max, value));
}

int Utils::clamp(int value, int min, int max) {
  return std::max(min, std::min(max, value));
}

double Utils::lerp(double a, double b, double t) { return a + (b - a) * t; }

float Utils::lerp(float a, float b, float t) { return a + (b - a) * t; }

double Utils::mapRange(double value, double inMin, double inMax, double outMin,
                       double outMax) {
  return outMin + (outMax - outMin) * ((value - inMin) / (inMax - inMin));
}

QString Utils::resolutionToString(int width, int height) {
  return QString("%1x%2").arg(width).arg(height);
}

QString Utils::fpsToString(double fps) { return QString::number(fps, 'f', 2); }

QString Utils::codecToString(int codecId) {
  const AVCodec *codec = avcodec_find_decoder(static_cast<AVCodecID>(codecId));
  if (codec) {
    return QString(codec->name);
  }
  return "Unknown";
}

QString Utils::sanitizeFilePath(const QString &path) {
  return QDir::fromNativeSeparators(path);
}

} // namespace cinemastudio
