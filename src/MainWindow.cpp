#include "MainWindow.h"
#include "AudioMixer.h"
#include "EffectsPanel.h"
#include "ExportDialog.h"
#include "FilterPanel.h"
#include "MediaLibrary.h"
#include "PropertiesPanel.h"
#include "Timeline.h"
#include "UndoRedoManager.h"
#include "VideoPlayer.h"
#include "core/ProjectFile.h"
#include "core/Track.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>

namespace cinemastudio {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle("CinemaStudio");
  resize(1400, 850);
  setWindowIcon(QIcon(":/icons/icon.png"));

  // Load dark theme
  QFile themeFile(":/theme.qss");
  if (themeFile.open(QFile::ReadOnly)) {
    QString theme = QLatin1String(themeFile.readAll());
    qApp->setStyleSheet(theme);
  }

  createActions();
  setupUI();
  loadSettings();

  auto project = std::make_unique<ProjectFile>();
  project->addTrack(TrackType::Video, "Video 1");
  project->addTrack(TrackType::Audio, "Audio 1");
  setCurrentProject(std::move(project));
}

MainWindow::~MainWindow() { saveSettings(); }

void MainWindow::setupUI() {
  // Central widget
  QWidget *central = new QWidget(this);
  setCentralWidget(central);
  auto *mainLayout = new QVBoxLayout(central);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Splitter for video player and timeline
  QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);

  // Video player
  m_videoPlayer = new VideoPlayer(this);
  mainSplitter->addWidget(m_videoPlayer);

  // Timeline
  m_timeline = new Timeline(this);
  mainSplitter->addWidget(m_timeline);

  mainSplitter->setStretchFactor(0, 3);
  mainSplitter->setStretchFactor(1, 2);
  mainSplitter->setSizes({height() * 3 / 5, height() * 2 / 5});

  mainLayout->addWidget(mainSplitter);

  setupMenus();
  setupToolbar();
  setupDockWidgets();
  setupStatusBar();
}

void MainWindow::setupMenus() {
  m_fileMenu = menuBar()->addMenu(tr("&File"));
  m_fileMenu->addAction(m_newAction);
  m_fileMenu->addAction(m_openAction);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_saveAction);
  m_fileMenu->addAction(m_saveAsAction);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_importAction);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(m_exportAction);

  m_editMenu = menuBar()->addMenu(tr("&Edit"));
  m_editMenu->addAction(m_undoAction);
  m_editMenu->addAction(m_redoAction);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_cutAction);
  m_editMenu->addAction(m_copyAction);
  m_editMenu->addAction(m_pasteAction);
  m_editMenu->addAction(m_deleteAction);
  m_editMenu->addSeparator();
  m_editMenu->addAction(m_splitAction);

  m_viewMenu = menuBar()->addMenu(tr("&View"));
  m_viewMenu->addAction(m_zoomInAction);
  m_viewMenu->addAction(m_zoomOutAction);
  m_viewMenu->addSeparator();
  m_viewMenu->addAction(m_fullscreenAction);

  m_helpMenu = menuBar()->addMenu(tr("&Help"));
  m_helpMenu->addAction(tr("&About CinemaStudio"), this, []() {
    QMessageBox::about(
        nullptr, "About CinemaStudio",
        "<h2>CinemaStudio 1.0</h2>"
        "<p>A modern, full-featured video editor built with C++ and Qt.</p>"
        "<p><b>Features:</b></p>"
        "<ul>"
        "<li>Multi-track timeline with drag-and-drop</li>"
        "<li>Real-time video preview</li>"
        "<li>Effects and transitions</li>"
        "<li>Audio mixing</li>"
        "<li>Color correction</li>"
        "<li>Text and titles</li>"
        "<li>Export to MP4, AVI, MOV, MKV</li>"
        "</ul>"
        "<p>Tech: C++17, Qt 6, FFmpeg, OpenGL</p>"
        "<p>© 2026 CinemaStudio — MIT License</p>");
  });
}

void MainWindow::setupToolbar() {
  QToolBar *toolbar = addToolBar("Main");
  toolbar->setMovable(false);
  toolbar->setIconSize(QSize(20, 20));
  toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  toolbar->setStyleSheet("QToolBar { spacing: 6px; padding: 6px; }");

  toolbar->addAction(m_newAction);
  toolbar->addAction(m_openAction);
  toolbar->addAction(m_saveAction);
  toolbar->addSeparator();
  toolbar->addAction(m_undoAction);
  toolbar->addAction(m_redoAction);
  toolbar->addSeparator();
  toolbar->addAction(m_importAction);
  toolbar->addAction(m_exportAction);

  // Edit mode toolbar
  QToolBar *editToolbar = addToolBar("Edit Tools");
  editToolbar->setMovable(false);
  editToolbar->setIconSize(QSize(20, 20));
  editToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  editToolbar->addAction(m_selectModeAction);
  editToolbar->addAction(m_cutModeAction);
  editToolbar->addAction(m_trimModeAction);
  editToolbar->addAction(m_splitModeAction);
  editToolbar->addAction(m_handModeAction);
}

void MainWindow::setupDockWidgets() {
  // Media Library (left)
  m_mediaLibrary = new MediaLibrary(this);
  QDockWidget *mediaDock = new QDockWidget("  Media Library", this);
  mediaDock->setFeatures(QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
  mediaDock->setWidget(m_mediaLibrary);
  addDockWidget(Qt::LeftDockWidgetArea, mediaDock);

  // Effects Panel (right)
  m_effectsPanel = new EffectsPanel(this);
  QDockWidget *effectsDock = new QDockWidget("  Effects", this);
  effectsDock->setFeatures(QDockWidget::DockWidgetMovable |
                           QDockWidget::DockWidgetFloatable);
  effectsDock->setWidget(m_effectsPanel);
  addDockWidget(Qt::RightDockWidgetArea, effectsDock);

  // Properties Panel (right)
  m_propertiesPanel = new PropertiesPanel(this);
  QDockWidget *propsDock = new QDockWidget("  Properties", this);
  propsDock->setFeatures(QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
  propsDock->setWidget(m_propertiesPanel);
  addDockWidget(Qt::RightDockWidgetArea, propsDock);
  tabifyDockWidget(effectsDock, propsDock);
  effectsDock->raise();

  // Audio Mixer (bottom)
  m_audioMixer = new AudioMixer(this);
  QDockWidget *audioDock = new QDockWidget("  Audio Mixer", this);
  audioDock->setFeatures(QDockWidget::DockWidgetMovable |
                         QDockWidget::DockWidgetFloatable);
  audioDock->setWidget(m_audioMixer);
  audioDock->setMinimumHeight(120);
  addDockWidget(Qt::BottomDockWidgetArea, audioDock);
}

void MainWindow::setupStatusBar() {
  QStatusBar *status = statusBar();
  status->addWidget(new QLabel("CinemaStudio — Ready"));
}

void MainWindow::createActions() {
  m_newAction = new QAction("📄", tr("&New"), this);
  m_newAction->setShortcuts(QKeySequence::New);
  m_newAction->setToolTip("New Project (Ctrl+N)");
  connect(m_newAction, &QAction::triggered, this, &MainWindow::newProject);

  m_openAction = new QAction("📂", tr("&Open..."), this);
  m_openAction->setShortcuts(QKeySequence::Open);
  m_openAction->setToolTip("Open Project (Ctrl+O)");
  connect(m_openAction, &QAction::triggered, this, &MainWindow::openProject);

  m_saveAction = new QAction("💾", tr("&Save"), this);
  m_saveAction->setShortcuts(QKeySequence::Save);
  m_saveAction->setToolTip("Save Project (Ctrl+S)");
  connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveProject);

  m_saveAsAction = new QAction(tr("Save &As..."), this);
  m_saveAsAction->setShortcuts(QKeySequence::SaveAs);
  connect(m_saveAsAction, &QAction::triggered, this,
          &MainWindow::saveProjectAs);

  m_exportAction = new QAction("🎬", tr("&Export Video..."), this);
  m_exportAction->setShortcut(QKeySequence("Ctrl+E"));
  m_exportAction->setToolTip("Export Video (Ctrl+E)");
  connect(m_exportAction, &QAction::triggered, this, &MainWindow::exportVideo);

  m_importAction = new QAction("📥", tr("&Import Media..."), this);
  m_importAction->setShortcut(QKeySequence("Ctrl+I"));
  m_importAction->setToolTip("Import Media (Ctrl+I)");
  connect(m_importAction, &QAction::triggered, this, &MainWindow::importMedia);

  m_undoAction = new QAction("↩️", tr("&Undo"), this);
  m_undoAction->setShortcuts(QKeySequence::Undo);
  connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);

  m_redoAction = new QAction("↪️", tr("&Redo"), this);
  m_redoAction->setShortcuts(QKeySequence::Redo);
  connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);

  m_splitAction = new QAction("✂️", tr("Split at Playhead"), this);
  m_splitAction->setShortcut(QKeySequence("Ctrl+B"));
  m_splitAction->setToolTip("Split Clip at Playhead (Ctrl+B)");
  connect(m_splitAction, &QAction::triggered, this,
          &MainWindow::splitClipAtPlayhead);

  m_cutAction = new QAction(tr("Cu&t"), this);
  m_cutAction->setShortcuts(QKeySequence::Cut);
  connect(m_cutAction, &QAction::triggered, this, &MainWindow::cutClip);

  m_copyAction = new QAction(tr("&Copy"), this);
  m_copyAction->setShortcuts(QKeySequence::Copy);
  connect(m_copyAction, &QAction::triggered, this, &MainWindow::copyClip);

  m_pasteAction = new QAction(tr("&Paste"), this);
  m_pasteAction->setShortcuts(QKeySequence::Paste);
  connect(m_pasteAction, &QAction::triggered, this, &MainWindow::pasteClip);

  m_deleteAction = new QAction(tr("&Delete"), this);
  m_deleteAction->setShortcut(QKeySequence::Delete);
  connect(m_deleteAction, &QAction::triggered, this, &MainWindow::deleteClip);

  m_zoomInAction = new QAction("🔍+", tr("Zoom &In"), this);
  m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
  connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

  m_zoomOutAction = new QAction("🔍-", tr("Zoom &Out"), this);
  m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

  m_fullscreenAction = new QAction(tr("F&ullscreen"), this);
  m_fullscreenAction->setShortcut(QKeySequence("F11"));
  m_fullscreenAction->setCheckable(true);
  connect(m_fullscreenAction, &QAction::triggered, this,
          &MainWindow::toggleFullscreen);

  // Edit mode group
  m_editModeGroup = new QActionGroup(this);
  m_editModeGroup->setExclusive(true);

  m_selectModeAction = new QAction("🔘", tr("Select"), this);
  m_selectModeAction->setCheckable(true);
  m_selectModeAction->setChecked(true);
  m_selectModeAction->setShortcut(QKeySequence("V"));
  m_editModeGroup->addAction(m_selectModeAction);
  connect(m_selectModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Select); });

  m_cutModeAction = new QAction("✂️", tr("Cut"), this);
  m_cutModeAction->setCheckable(true);
  m_cutModeAction->setShortcut(QKeySequence("C"));
  m_editModeGroup->addAction(m_cutModeAction);
  connect(m_cutModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Cut); });

  m_trimModeAction = new QAction("📏", tr("Trim"), this);
  m_trimModeAction->setCheckable(true);
  m_trimModeAction->setShortcut(QKeySequence("T"));
  m_editModeGroup->addAction(m_trimModeAction);
  connect(m_trimModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Trim); });

  m_splitModeAction = new QAction("🔪", tr("Split"), this);
  m_splitModeAction->setCheckable(true);
  m_splitModeAction->setShortcut(QKeySequence("S"));
  m_editModeGroup->addAction(m_splitModeAction);
  connect(m_splitModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Split); });

  m_handModeAction = new QAction("✋", tr("Hand"), this);
  m_handModeAction->setCheckable(true);
  m_handModeAction->setShortcut(QKeySequence("H"));
  m_editModeGroup->addAction(m_handModeAction);
  connect(m_handModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Hand); });
}

void MainWindow::newProject() {
  auto project = std::make_unique<ProjectFile>();
  project->addTrack(TrackType::Video, "Video 1");
  project->addTrack(TrackType::Audio, "Audio 1");
  setCurrentProject(std::move(project));
  setWindowTitle("CinemaStudio — Untitled Project");
}

void MainWindow::openProject() {
  QString filePath = QFileDialog::getOpenFileName(
      this, "Open Project", QString(),
      "CinemaStudio Project (*.csp);;All Files (*)");
  if (filePath.isEmpty())
    return;
  auto project = std::make_unique<ProjectFile>();
  if (project->loadFromFile(filePath)) {
    setCurrentProject(std::move(project));
    setWindowTitle("CinemaStudio — " + QFileInfo(filePath).fileName());
  } else {
    QMessageBox::critical(this, "Error", "Failed to open project file.");
  }
}

void MainWindow::saveProject() { saveProjectAs(); }

void MainWindow::saveProjectAs() {
  QString filePath = QFileDialog::getSaveFileName(
      this, "Save Project", QString(), "CinemaStudio Project (*.csp)");
  if (filePath.isEmpty())
    return;
  if (!filePath.endsWith(".csp"))
    filePath += ".csp";
  if (m_currentProject->saveToFile(filePath)) {
    statusBar()->showMessage("✅ Project saved", 3000);
  }
}

void MainWindow::exportVideo() {
  if (!m_exportDialog)
    m_exportDialog = new ExportDialog(this);
  m_exportDialog->setProject(m_currentProject.get());
  m_exportDialog->exec();
}

void MainWindow::undo() {
  if (m_undoRedoManager)
    m_undoRedoManager->undo();
}
void MainWindow::redo() {
  if (m_undoRedoManager)
    m_undoRedoManager->redo();
}
void MainWindow::cutClip() {}
void MainWindow::copyClip() {}
void MainWindow::pasteClip() {}
void MainWindow::deleteClip() {}
void MainWindow::selectAll() {}

void MainWindow::zoomIn() {
  if (m_timeline)
    m_timeline->zoomIn();
}
void MainWindow::zoomOut() {
  if (m_timeline)
    m_timeline->zoomOut();
}

void MainWindow::toggleFullscreen() {
  if (isFullScreen())
    showNormal();
  else
    showFullScreen();
}

void MainWindow::setEditMode(EditMode mode) {
  m_currentEditMode = mode;
  if (m_timeline)
    m_timeline->setEditMode(mode);
}

void MainWindow::splitClipAtPlayhead() {
  if (m_timeline)
    m_timeline->splitClipAtPlayhead();
}

void MainWindow::importMedia() {
  QStringList files = QFileDialog::getOpenFileNames(
      this, "Import Media", QString(),
      "Video Files (*.mp4 *.avi *.mov *.mkv *.wmv);;"
      "Audio Files (*.mp3 *.wav *.aac *.flac);;"
      "Image Files (*.png *.jpg *.jpeg *.bmp);;"
      "All Files (*)");
  if (!files.isEmpty() && m_mediaLibrary)
    m_mediaLibrary->addFiles(files);
}

void MainWindow::importMediaFiles(const QStringList &files) {
  if (m_mediaLibrary)
    m_mediaLibrary->addFiles(files);
}

void MainWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  event->accept();
}

void MainWindow::setCurrentProject(std::unique_ptr<ProjectFile> project) {
  m_currentProject = std::move(project);
  if (m_videoPlayer)
    m_videoPlayer->setProject(m_currentProject.get());
  if (m_timeline)
    m_timeline->setProject(m_currentProject.get());
  if (m_audioMixer)
    m_audioMixer->setProject(m_currentProject.get());
}

void MainWindow::loadSettings() {
  QSettings s("CinemaStudio", "CinemaStudio");
  restoreGeometry(s.value("geometry").toByteArray());
  restoreState(s.value("windowState").toByteArray());
}

void MainWindow::saveSettings() {
  QSettings s("CinemaStudio", "CinemaStudio");
  s.setValue("geometry", saveGeometry());
  s.setValue("windowState", saveState());
}

} // namespace cinemastudio
