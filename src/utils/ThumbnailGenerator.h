#pragma once

#include <QString>
#include <QImage>
#include <QVector>
#include <functional>

namespace cinemastudio {

class ThumbnailGenerator {
public:
    ThumbnailGenerator();
    ~ThumbnailGenerator();

    // Generate thumbnails for a video file
    QVector<QImage> generateThumbnails(const QString& videoPath, int count = 10, 
                                        int width = 160, int height = 90);
    
    // Generate single thumbnail at specific time
    QImage generateThumbnail(const QString& videoPath, double timeSeconds, 
                             int width = 160, int height = 90);

    // Cache management
    void clearCache();
    void removeFromCache(const QString& filePath);
    QVector<QImage>* getCachedThumbnails(const QString& filePath);

    // Progress callback
    using ProgressCallback = std::function<void(int current, int total)>;
    void setProgressCallback(ProgressCallback callback) { m_progressCallback = callback; }

private:
    struct CacheEntry {
        QVector<QImage> thumbnails;
        QString filePath;
    };

    QVector<CacheEntry> m_cache;
    ProgressCallback m_progressCallback;
};

} // namespace cinemastudio
