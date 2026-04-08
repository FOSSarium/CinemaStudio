#include "VideoPlayer.h"
#include "core/Clip.h"
#include "core/ProjectFile.h"
#include "core/Track.h"
#include <QMouseEvent>
#include <QStyle>

namespace cinemastudio {

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent) {
  setObjectName("videoPlayer");
  setupUI();
}

VideoPlayer::~VideoPlayer() = default;

void VideoPlayer::setupUI() {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Video canvas
  m_videoCanvas = new QWidget(this);
  m_videoCanvas->setMinimumSize(320, 180);
  m_videoCanvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_videoCanvas->setStyleSheet("background-color: #000000;");
  layout->addWidget(m_videoCanvas, 1);

  // Transport controls
  m_transportBar = new QWidget(this);
  m_transportBar->setObjectName("transportControls");
  m_transportBar->setFixedHeight(50);
  layout->addWidget(m_transportBar);

  auto *transportLayout = new QHBoxLayout(m_transportBar);
  transportLayout->setContentsMargins(10, 4, 10, 4);
  transportLayout->setSpacing(8);

  // Skip back
  m_skipBackBtn = new QPushButton("⏮", this);
  m_skipBackBtn->setFixedSize(32, 32);
  m_skipBackBtn->setCursor(Qt::PointingHandCursor);
  m_skipBackBtn->setToolTip("Skip to start");
  connect(m_skipBackBtn, &QPushButton::clicked, this, &VideoPlayer::onSkipBack);

  // Seek backward
  m_seekBackBtn = new QPushButton("◀◀", this);
  m_seekBackBtn->setFixedSize(32, 32);
  m_seekBackBtn->setCursor(Qt::PointingHandCursor);
  m_seekBackBtn->setToolTip("Rewind 1s");
  connect(m_seekBackBtn, &QPushButton::clicked, this,
          &VideoPlayer::onSeekBackward);

  // Play/Pause
  m_playPauseBtn = new QPushButton("▶", this);
  m_playPauseBtn->setFixedSize(40, 40);
  m_playPauseBtn->setCursor(Qt::PointingHandCursor);
  m_playPauseBtn->setStyleSheet("background-color: #2196f3; color: white; "
                                "border-radius: 20px; font-size: 16px;");
  m_playPauseBtn->setToolTip("Play/Pause");
  connect(m_playPauseBtn, &QPushButton::clicked, this,
          &VideoPlayer::onPlayPause);

  // Seek forward
  m_seekForwardBtn = new QPushButton("▶▶", this);
  m_seekForwardBtn->setFixedSize(32, 32);
  m_seekForwardBtn->setCursor(Qt::PointingHandCursor);
  m_seekForwardBtn->setToolTip("Forward 1s");
  connect(m_seekForwardBtn, &QPushButton::clicked, this,
          &VideoPlayer::onSeekForward);

  // Skip forward
  m_skipForwardBtn = new QPushButton("⏭", this);
  m_skipForwardBtn->setFixedSize(32, 32);
  m_skipForwardBtn->setCursor(Qt::PointingHandCursor);
  m_skipForwardBtn->setToolTip("Skip to end");
  connect(m_skipForwardBtn, &QPushButton::clicked, this,
          &VideoPlayer::onSkipForward);

  transportLayout->addWidget(m_skipBackBtn);
  transportLayout->addWidget(m_seekBackBtn);
  transportLayout->addWidget(m_playPauseBtn);
  transportLayout->addWidget(m_seekForwardBtn);
  transportLayout->addWidget(m_skipForwardBtn);

  // Timeline slider
  m_timelineSlider = new QSlider(Qt::Horizontal, this);
  m_timelineSlider->setRange(0, 1000);
  m_timelineSlider->setValue(0);
  m_timelineSlider->setCursor(Qt::PointingHandCursor);
  m_timelineSlider->setToolTip("Seek");
  connect(m_timelineSlider, &QSlider::valueChanged, this,
          &VideoPlayer::onTimelineSliderChanged);
  transportLayout->addWidget(m_timelineSlider, 1);

  // Timecode
  m_timecodeLabel = new QLabel("00:00:00.00", this);
  m_timecodeLabel->setObjectName("timecode");
  m_timecodeLabel->setAlignment(Qt::AlignCenter);
  m_timecodeLabel->setFixedWidth(120);
  transportLayout->addWidget(m_timecodeLabel);

  m_durationLabel = new QLabel("/ 00:00:00.00", this);
  m_durationLabel->setAlignment(Qt::AlignCenter);
  m_durationLabel->setStyleSheet("color: #888888; font-size: 12px;");
  m_durationLabel->setFixedWidth(120);
  transportLayout->addWidget(m_durationLabel);

  // Status label
  m_statusLabel = new QLabel("Ready", this);
  m_statusLabel->setAlignment(Qt::AlignRight);
  m_statusLabel->setStyleSheet(
      "color: #4caf50; font-size: 11px; padding-right: 8px;");
  m_statusLabel->setFixedWidth(100);
  transportLayout->addWidget(m_statusLabel);

  // Timer
  m_updateTimer = new QTimer(this);
  connect(m_updateTimer, &QTimer::timeout, this, &VideoPlayer::updateFrame);
}

void VideoPlayer::setProject(ProjectFile *project) {
  m_project = project;
  if (project) {
    m_duration = project->getTimelineDuration();
    updateTimecode();
  }
}

void VideoPlayer::setDuration(double duration) {
  m_duration = duration;
  updateTimecode();
}

void VideoPlayer::play() {
  if (m_isPlaying)
    return;
  m_isPlaying = true;
  m_playPauseBtn->setText("⏸");
  m_playPauseBtn->setStyleSheet("background-color: #f44336; color: white; "
                                "border-radius: 20px; font-size: 16px;");
  m_updateTimer->start(1000 / m_fps);
  m_statusLabel->setText("Playing");
  m_statusLabel->setStyleSheet(
      "color: #2196f3; font-size: 11px; padding-right: 8px;");
  emit playStateChanged(true);
}

void VideoPlayer::pause() {
  m_isPlaying = false;
  m_playPauseBtn->setText("▶");
  m_playPauseBtn->setStyleSheet("background-color: #2196f3; color: white; "
                                "border-radius: 20px; font-size: 16px;");
  m_updateTimer->stop();
  m_statusLabel->setText("Paused");
  m_statusLabel->setStyleSheet(
      "color: #ff9800; font-size: 11px; padding-right: 8px;");
  emit playStateChanged(false);
}

void VideoPlayer::stop() {
  m_isPlaying = false;
  m_currentTime = 0.0;
  m_playPauseBtn->setText("▶");
  m_playPauseBtn->setStyleSheet("background-color: #2196f3; color: white; "
                                "border-radius: 20px; font-size: 16px;");
  m_updateTimer->stop();
  m_timelineSlider->setValue(0);
  m_statusLabel->setText("Stopped");
  m_statusLabel->setStyleSheet(
      "color: #888888; font-size: 11px; padding-right: 8px;");
  emit playStateChanged(false);
  emit timeChanged(0.0);
  m_videoCanvas->update();
}

void VideoPlayer::seek(double time) {
  m_currentTime = std::max(0.0, time);
  if (m_project)
    m_currentTime = std::min(m_currentTime, m_duration);

  if (m_duration > 0) {
    m_timelineSlider->setValue(
        static_cast<int>(m_currentTime / m_duration * 1000));
  }
  emit timeChanged(m_currentTime);
  updateTimecode();
  renderCurrentFrame();
  m_videoCanvas->update();
}

void VideoPlayer::renderCurrentFrame() {
  if (!m_project || m_duration == 0) {
    m_currentFrame = QImage(m_videoCanvas->size(), QImage::Format_RGB32);
    m_currentFrame.fill(QColor("#0a0a0a"));
    return;
  }

  QImage frame(m_videoCanvas->size(), QImage::Format_RGB32);
  frame.fill(QColor("#000000"));

  auto videoTracks = m_project->getVideoTracks();
  for (int i = videoTracks.size() - 1; i >= 0; --i) {
    auto track = videoTracks[i];
    if (!track->isVisible())
      continue;
    auto clip = track->getClipAtTime(m_currentTime);
    if (clip && !clip->getFilePath().isEmpty()) {
      QImage thumb = clip->getThumbnail();
      if (!thumb.isNull()) {
        QPainter painter(&frame);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        QRect drawRect = frame.rect();
        drawRect.setSize(
            thumb.size().scaled(drawRect.size(), Qt::KeepAspectRatio));
        drawRect.moveCenter(frame.rect().center());
        painter.drawImage(drawRect, thumb);
      }
      break;
    }
  }
  m_currentFrame = frame;
}

void VideoPlayer::onPlayPause() {
  if (m_isPlaying)
    pause();
  else
    play();
}

void VideoPlayer::onSeekBackward() { seek(m_currentTime - 1.0); }
void VideoPlayer::onSeekForward() { seek(m_currentTime + 1.0); }
void VideoPlayer::onSkipBack() { seek(0); }
void VideoPlayer::onSkipForward() { seek(m_duration); }

void VideoPlayer::onTimelineSliderChanged(int value) {
  seek(value / 1000.0 * m_duration);
}

void VideoPlayer::updateFrame() {
  if (!m_isPlaying)
    return;
  m_currentTime += 1.0 / m_fps;
  if (m_project && m_currentTime >= m_duration) {
    stop();
    return;
  }
  m_timelineSlider->setValue(
      static_cast<int>(m_currentTime / m_duration * 1000));
  emit timeChanged(m_currentTime);
  updateTimecode();
  renderCurrentFrame();
  m_videoCanvas->update();
}

void VideoPlayer::paintEvent(QPaintEvent *) {
  QPainter painter(m_videoCanvas);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);

  if (m_currentFrame.isNull() || m_currentFrame.width() == 0) {
    painter.fillRect(m_videoCanvas->rect(), QColor("#0a0a0a"));
    painter.setPen(QColor("#555555"));
    QFont font = painter.font();
    font.setPointSize(18);
    painter.setFont(font);
    painter.drawText(m_videoCanvas->rect(), Qt::AlignCenter,
                     "No Media\nImport video files to begin editing");
  } else {
    QRect drawRect = m_videoCanvas->rect();
    QImage scaled = m_currentFrame.scaled(drawRect.size(), Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation);
    QRect imageRect = scaled.rect();
    imageRect.moveCenter(drawRect.center());
    painter.drawImage(imageRect, scaled);
  }
  drawOverlay(painter);
}

void VideoPlayer::drawOverlay(QPainter &painter) {
  // Semi-transparent overlay when not playing
  if (!m_isPlaying && m_duration > 0) {
    painter.fillRect(m_videoCanvas->rect(), QColor(0, 0, 0, 80));
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPointSize(36);
    painter.setFont(font);
    painter.drawText(m_videoCanvas->rect(), Qt::AlignCenter, "▶");
  }
}

void VideoPlayer::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (m_isPlaying)
      pause();
    else
      play();
  }
}

void VideoPlayer::wheelEvent(QWheelEvent *event) {
  if (event->modifiers() & Qt::ControlModifier) {
    // Adjust volume or other parameter
  } else {
    seek(m_currentTime + event->angleDelta().y() / 1200.0);
  }
}

void VideoPlayer::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  if (!m_currentFrame.isNull())
    renderCurrentFrame();
}

double VideoPlayer::getDuration() const { return m_duration; }

void VideoPlayer::updateTimecode() {
  auto formatTime = [](double secs) {
    int h = (int)secs / 3600;
    int m = ((int)secs % 3600) / 60;
    int s = (int)secs % 60;
    int ms = (int)((secs - (int)secs) * 100);
    return QString("%1:%2:%3.%4")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'))
        .arg(ms, 2, 10, QChar('0'));
  };
  m_timecodeLabel->setText(formatTime(m_currentTime));
  m_durationLabel->setText("/ " + formatTime(m_duration));
}

} // namespace cinemastudio
