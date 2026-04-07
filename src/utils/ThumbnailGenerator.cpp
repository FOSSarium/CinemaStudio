#include "ThumbnailGenerator.h"
#include "ffmpeg/VideoDecoder.h"
#include <QDebug>

namespace cinemastudio {

ThumbnailGenerator::ThumbnailGenerator() = default;
ThumbnailGenerator::~ThumbnailGenerator() = default;

QVector<QImage> ThumbnailGenerator::generateThumbnails(const QString& videoPath, int count,
                                                        int width, int height) {
    // Check cache
    for (const auto& entry : m_cache) {
        if (entry.filePath == videoPath) {
            return entry.thumbnails;
        }
    }

    // Generate thumbnails
    QVector<QImage> thumbnails;
    VideoDecoder decoder;
    
    if (!decoder.open(videoPath)) {
        qCritical() << "Failed to open video for thumbnails:" << videoPath;
        return thumbnails;
    }

    double duration = decoder.getDuration();
    double interval = duration / (count + 1);

    for (int i = 1; i <= count; i++) {
        double time = interval * i;
        QImage thumb = decoder.generateThumbnail(time, width, height);
        if (!thumb.isNull()) {
            thumbnails.append(thumb);
        }

        if (m_progressCallback) {
            m_progressCallback(i, count);
        }
    }

    // Cache result
    if (!thumbnails.isEmpty()) {
        m_cache.append({thumbnails, videoPath});
    }

    return thumbnails;
}

QImage ThumbnailGenerator::generateThumbnail(const QString& videoPath, double timeSeconds,
                                              int width, int height) {
    VideoDecoder decoder;
    if (!decoder.open(videoPath)) {
        return QImage();
    }

    return decoder.generateThumbnail(timeSeconds, width, height);
}

void ThumbnailGenerator::clearCache() {
    m_cache.clear();
}

void ThumbnailGenerator::removeFromCache(const QString& filePath) {
    m_cache.erase(
        std::remove_if(m_cache.begin(), m_cache.end(),
            [&filePath](const CacheEntry& entry) {
                return entry.filePath == filePath;
            }),
        m_cache.end()
    );
}

QVector<QImage>* ThumbnailGenerator::getCachedThumbnails(const QString& filePath) {
    for (auto& entry : m_cache) {
        if (entry.filePath == filePath) {
            return &entry.thumbnails;
        }
    }
    return nullptr;
}

} // namespace cinemastudio
