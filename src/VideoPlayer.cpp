#include "VideoPlayer.h"
#include "core/ProjectFile.h"
#include "core/Clip.h"
#include "core/Track.h"
#include "ffmpeg/VideoDecoder.h"
#include "core/TextOverlay.h"
#include <QVBoxLayout>
#include <QMouseEvent>

namespace cinemastudio {

VideoPlayer::VideoPlayer(QWidget* parent)
    : QWidget(parent) {
    setMinimumSize(320, 180);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: #1a1a1a;");

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &VideoPlayer::updateFrame);
}

VideoPlayer::~VideoPlayer() {
    stop();
}

void VideoPlayer::setProject(ProjectFile* project) {
    m_project = project;
}

void VideoPlayer::play() {
    if (m_isPlaying) return;
    
    m_isPlaying = true;
    m_updateTimer->start(1000 / m_fps);
    emit playStateChanged(true);
}

void VideoPlayer::pause() {
    m_isPlaying = false;
    m_updateTimer->stop();
    emit playStateChanged(false);
}

void VideoPlayer::stop() {
    m_isPlaying = false;
    m_updateTimer->stop();
    m_currentTime = 0.0;
    emit playStateChanged(false);
    emit timeChanged(0.0);
    update();
}

void VideoPlayer::seek(double timeSeconds) {
    m_currentTime = std::max(0.0, timeSeconds);
    if (m_project) {
        m_currentTime = std::min(m_currentTime, m_project->getTimelineDuration());
    }
    emit timeChanged(m_currentTime);
    renderCurrentFrame();
    update();
}

double VideoPlayer::getDuration() const {
    return m_project ? m_project->getTimelineDuration() : 0.0;
}

void VideoPlayer::setZoom(float zoom) {
    m_zoom = std::clamp(zoom, 0.1f, 5.0f);
    update();
}

void VideoPlayer::updateFrame() {
    if (!m_isPlaying) return;

    m_currentTime += 1.0 / m_fps;
    
    if (m_project && m_currentTime >= m_project->getTimelineDuration()) {
        stop();
        return;
    }

    emit timeChanged(m_currentTime);
    renderCurrentFrame();
    update();
}

void VideoPlayer::renderCurrentFrame() {
    if (!m_project) {
        m_currentFrame = QImage(size(), QImage::Format_RGB32);
        m_currentFrame.fill(QColor("#1a1a1a"));
        return;
    }

    // Find the active clip at current time
    QImage frame(size(), QImage::Format_RGB32);
    frame.fill(QColor("#000000"));

    auto videoTracks = m_project->getVideoTracks();
    
    for (int trackIdx = videoTracks.size() - 1; trackIdx >= 0; trackIdx--) {
        auto track = videoTracks[trackIdx];
        if (!track->isVisible()) continue;

        auto clip = track->getClipAtTime(m_currentTime);
        if (clip && !clip->getFilePath().isEmpty()) {
            // Load frame from video
            VideoDecoder decoder;
            if (decoder.open(clip->getFilePath())) {
                double clipTime = m_currentTime - clip->getProperties().timelinePosition + clip->getProperties().startTime;
                QImage videoFrame = decoder.generateThumbnail(clipTime, width(), height());
                if (!videoFrame.isNull()) {
                    QPainter painter(&frame);
                    painter.drawImage(0, 0, videoFrame);
                }
            }
        }
    }

    m_currentFrame = frame;
}

void VideoPlayer::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    drawFrame(painter);
    drawOverlay(painter);
}

void VideoPlayer::drawFrame(QPainter& painter) {
    if (m_currentFrame.isNull()) {
        painter.fillRect(rect(), QColor("#1a1a1a"));
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "No Media");
        return;
    }

    QRect drawRect = rect();
    if (m_zoom != 1.0f) {
        int w = m_currentFrame.width() * m_zoom;
        int h = m_currentFrame.height() * m_zoom;
        drawRect = QRect((width() - w) / 2, (height() - h) / 2, w, h);
    }

    painter.drawImage(drawRect, m_currentFrame, m_currentFrame.rect());
}

void VideoPlayer::drawOverlay(QPainter& painter) {
    if (!m_isPlaying) {
        // Draw play button overlay
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 100));
        painter.drawEllipse(QRectF(width()/2 - 30, height()/2 - 30, 60, 60));
        
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize(24);
        painter.setFont(font);
        painter.drawText(QRectF(width()/2 - 30, height()/2 - 30, 60, 60), Qt::AlignCenter, "▶");
    }

    // Draw timecode
    QString timecode = QString("%1 / %2")
        .arg(m_currentTime, 0, 'f', 2)
        .arg(getDuration(), 0, 'f', 2);
    
    painter.setPen(Qt::white);
    painter.setFont(QFont("Monospace", 10));
    QRect timeRect = QRect(10, height() - 30, 150, 20);
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.drawRect(timeRect);
    painter.drawText(timeRect, Qt::AlignCenter, timecode);
}

void VideoPlayer::drawTextOverlays(QPainter& painter) {
    if (!m_project) return;

    auto textTracks = m_project->getTextTracks();
    for (auto track : textTracks) {
        if (!track->isVisible()) continue;
        
        auto clip = track->getClipAtTime(m_currentTime);
        if (clip && clip->getType() == ClipType::Text) {
            // Draw text overlay
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 24, QFont::Bold));
            painter.drawText(rect(), Qt::AlignCenter, clip->getName());
        }
    }
}

void VideoPlayer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (m_isPlaying) {
            pause();
        } else {
            play();
        }
    }
    QWidget::mousePressEvent(event);
}

void VideoPlayer::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // Toggle fullscreen handled by MainWindow
    }
    QWidget::mouseDoubleClickEvent(event);
}

void VideoPlayer::wheelEvent(QWheelEvent* event) {
    // Zoom with mouse wheel
    float zoomDelta = event->angleDelta().y() / 1200.0f;
    setZoom(m_zoom + zoomDelta);
    QWidget::wheelEvent(event);
}

} // namespace cinemastudio
