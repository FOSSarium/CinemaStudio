#pragma once

#include <QString>
#include <QColor>
#include <QTime>

namespace cinemastudio {

class Utils {
public:
    // Time formatting
    static QString formatTime(double seconds);
    static QString formatTimeShort(double seconds);
    static double parseTime(const QString& timeStr);

    // File utilities
    static QString formatFileSize(qint64 bytes);
    static QString getFileNameWithoutExt(const QString& filePath);
    static QString getFileExtension(const QString& filePath);

    // Color utilities
    static QColor blendColors(const QColor& c1, const QColor& c2, float t);
    static QColor adjustBrightness(const QColor& color, float factor);
    static QColor adjustSaturation(const QColor& color, float factor);

    // Math utilities
    static double clamp(double value, double min, double max);
    static float clamp(float value, float min, float max);
    static int clamp(int value, int min, int max);
    static double lerp(double a, double b, double t);
    static float lerp(float a, float b, float t);
    static double mapRange(double value, double inMin, double inMax, double outMin, double outMax);

    // Video utilities
    static QString resolutionToString(int width, int height);
    static QString fpsToString(double fps);
    static QString codecToString(int codecId);

    // Common paths
    static QString sanitizeFilePath(const QString& path);
};

} // namespace cinemastudio
