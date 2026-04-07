#pragma once

#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <memory>

class QVideoWidget;
class QMediaPlayer;
class QGraphicsVideoItem;

namespace cinemastudio {

class ProjectFile;

class VideoPlayer : public QWidget {
    Q_OBJECT

public:
    VideoPlayer(QWidget* parent = nullptr);
    ~VideoPlayer();

    void setProject(ProjectFile* project);
    void play();
    void pause();
    void stop();
    bool isPlaying() const { return m_isPlaying; }

    void seek(double timeSeconds);
    double getCurrentTime() const { return m_currentTime; }
    double getDuration() const;

    void setZoom(float zoom);
    float getZoom() const { return m_zoom; }

signals:
    void timeChanged(double time);
    void playStateChanged(bool playing);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void updateFrame();

private:
    void drawFrame(QPainter& painter);
    void drawOverlay(QPainter& painter);
    void drawTextOverlays(QPainter& painter);
    void renderCurrentFrame();

    ProjectFile* m_project = nullptr;
    QImage m_currentFrame;
    QTimer* m_updateTimer = nullptr;
    
    double m_currentTime = 0.0;
    bool m_isPlaying = false;
    float m_zoom = 1.0f;
    
    // Playback
    int m_playbackTimer = 0;
    double m_fps = 30.0;
};

} // namespace cinemastudio
