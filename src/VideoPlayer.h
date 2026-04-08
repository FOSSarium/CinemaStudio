#pragma once

#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

namespace cinemastudio {

class ProjectFile;

class VideoPlayer : public QWidget {
  Q_OBJECT

public:
  explicit VideoPlayer(QWidget *parent = nullptr);
  ~VideoPlayer();

  void setProject(ProjectFile *project);
  void play();
  void pause();
  void stop();
  bool isPlaying() const { return m_isPlaying; }

  void seek(double timeSeconds);
  double getCurrentTime() const { return m_currentTime; }
  double getDuration() const;
  void setDuration(double duration);

signals:
  void timeChanged(double time);
  void playStateChanged(bool playing);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void onPlayPause();
  void onSeekBackward();
  void onSeekForward();
  void onSkipBack();
  void onSkipForward();
  void onTimelineSliderChanged(int value);
  void updateFrame();

private:
  void setupUI();
  void drawFrame(QPainter &painter);
  void drawOverlay(QPainter &painter);
  void renderCurrentFrame();
  void updateTimecode();

  ProjectFile *m_project = nullptr;
  QImage m_currentFrame;
  QTimer *m_updateTimer = nullptr;

  // UI Components
  QWidget *m_videoCanvas = nullptr;
  QWidget *m_transportBar = nullptr;
  QLabel *m_timecodeLabel = nullptr;
  QLabel *m_durationLabel = nullptr;
  QSlider *m_timelineSlider = nullptr;
  QPushButton *m_playPauseBtn = nullptr;
  QPushButton *m_skipBackBtn = nullptr;
  QPushButton *m_skipForwardBtn = nullptr;
  QPushButton *m_seekBackBtn = nullptr;
  QPushButton *m_seekForwardBtn = nullptr;
  QLabel *m_statusLabel = nullptr;

  double m_currentTime = 0.0;
  double m_duration = 0.0;
  bool m_isPlaying = false;
  double m_fps = 30.0;
  QString m_statusText;
};

} // namespace cinemastudio
