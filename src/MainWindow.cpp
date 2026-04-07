#include "MainWindow.h"
#include "AudioMixer.h"
#include "EffectsPanel.h"
#include "ExportDialog.h"
#include "FilterPanel.h"
#include "MediaLibrary.h"
#include "PropertiesPanel.h"
#include "Timeline.h"
#include "Toolbar.h"
#include "UndoRedoManager.h"
#include "VideoPlayer.h"
#include "core/ProjectFile.h"
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeySequence>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QVBoxLayout>

namespace cinemastudio {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

  setWindowTitle("CinemaStudio");
  resize(1600, 900);
  setWindowIcon(QIcon(":/icons/icon.png"));

  createActions();
  setupUI();
  loadSettings();

  // Create initial project
  auto project = std::make_unique<ProjectFile>();
  project->addTrack(TrackType::Video, "Video 1");
  project->addTrack(TrackType::Audio, "Audio 1");
  setCurrentProject(std::move(project));
}

MainWindow::~MainWindow() { saveSettings(); }

void MainWindow::setupUI() {
  // Central widget with main layout
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Video player (top)
  m_videoPlayer = new VideoPlayer(this);
  mainLayout->addWidget(m_videoPlayer, 3);

  // Timeline (bottom)
  m_timeline = new Timeline(this);
  mainLayout->addWidget(m_timeline, 2);

  setupMenus();
  setupToolbar();
  setupDockWidgets();
  setupStatusBar();
}

void MainWindow::setupMenus() {
  // File menu
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

  // Edit menu
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

  // View menu
  m_viewMenu = menuBar()->addMenu(tr("&View"));
  m_viewMenu->addAction(m_zoomInAction);
  m_viewMenu->addAction(m_zoomOutAction);
  m_viewMenu->addAction(m_fullscreenAction);

  // Help menu
  m_helpMenu = menuBar()->addMenu(tr("&Help"));
  m_helpMenu->addAction(tr("&About CinemaStudio"), this, []() {
    QMessageBox::about(
        nullptr, "About CinemaStudio",
        "<h2>CinemaStudio 1.0</h2>"
        "<p>A modern, full-featured video editor built with C++ and Qt.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Multi-track timeline</li>"
        "<li>Real-time preview</li>"
        "<li>Effects and transitions</li>"
        "<li>Audio mixing</li>"
        "<li>Color correction</li>"
        "<li>Text and titles</li>"
        "</ul>"
        "<p>© 2026 CinemaStudio</p>");
  });
}

void MainWindow::setupToolbar() {
  QToolBar *toolbar = addToolBar("Main Toolbar");
  toolbar->setMovable(false);
  toolbar->setIconSize(QSize(24, 24));

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
  editToolbar->setIconSize(QSize(24, 24));

  editToolbar->addAction(m_selectModeAction);
  editToolbar->addAction(m_cutModeAction);
  editToolbar->addAction(m_trimModeAction);
  editToolbar->addAction(m_splitModeAction);
  editToolbar->addAction(m_handModeAction);
}

void MainWindow::setupDockWidgets() {
  // Media Library (left)
  m_mediaLibrary = new MediaLibrary(this);
  QDockWidget *mediaDock = new QDockWidget("Media Library", this);
  mediaDock->setWidget(m_mediaLibrary);
  addDockWidget(Qt::LeftDockWidgetArea, mediaDock);

  // Effects Panel (right)
  m_effectsPanel = new EffectsPanel(this);
  QDockWidget *effectsDock = new QDockWidget("Effects", this);
  effectsDock->setWidget(m_effectsPanel);
  addDockWidget(Qt::RightDockWidgetArea, effectsDock);

  // Filter Panel (right)
  m_filterPanel = new FilterPanel(this);
  QDockWidget *filterDock = new QDockWidget("Filters", this);
  filterDock->setWidget(m_filterPanel);
  addDockWidget(Qt::RightDockWidgetArea, filterDock);
  tabifyDockWidget(effectsDock, filterDock);
  effectsDock->raise();

  // Audio Mixer (bottom)
  m_audioMixer = new AudioMixer(this);
  QDockWidget *audioDock = new QDockWidget("Audio Mixer", this);
  audioDock->setWidget(m_audioMixer);
  addDockWidget(Qt::BottomDockWidgetArea, audioDock);

  // Properties Panel (right)
  m_propertiesPanel = new PropertiesPanel(this);
  QDockWidget *propsDock = new QDockWidget("Properties", this);
  propsDock->setWidget(m_propertiesPanel);
  addDockWidget(Qt::RightDockWidgetArea, propsDock);
}

void MainWindow::setupStatusBar() {
  QStatusBar *status = statusBar();
  status->addWidget(new QLabel("Ready"));
}

void MainWindow::createActions() {
  // File actions
  m_newAction = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
  m_newAction->setShortcuts(QKeySequence::New);
  connect(m_newAction, &QAction::triggered, this, &MainWindow::newProject);

  m_openAction =
      new QAction(QIcon::fromTheme("document-open"), tr("&Open..."), this);
  m_openAction->setShortcuts(QKeySequence::Open);
  connect(m_openAction, &QAction::triggered, this, &MainWindow::openProject);

  m_saveAction =
      new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
  m_saveAction->setShortcuts(QKeySequence::Save);
  connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveProject);

  m_saveAsAction = new QAction(QIcon::fromTheme("document-save-as"),
                               tr("Save &As..."), this);
  m_saveAsAction->setShortcuts(QKeySequence::SaveAs);
  connect(m_saveAsAction, &QAction::triggered, this,
          &MainWindow::saveProjectAs);

  m_exportAction = new QAction(QIcon::fromTheme("media-record"),
                               tr("&Export Video..."), this);
  m_exportAction->setShortcut(QKeySequence("Ctrl+E"));
  connect(m_exportAction, &QAction::triggered, this, &MainWindow::exportVideo);

  m_importAction =
      new QAction(QIcon::fromTheme("list-add"), tr("&Import Media..."), this);
  m_importAction->setShortcut(QKeySequence("Ctrl+I"));
  connect(m_importAction, &QAction::triggered, this, &MainWindow::importMedia);

  // Edit actions
  m_undoAction = new QAction(QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
  m_undoAction->setShortcuts(QKeySequence::Undo);
  connect(m_undoAction, &QAction::triggered, this, &MainWindow::undo);

  m_redoAction = new QAction(QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
  m_redoAction->setShortcuts(QKeySequence::Redo);
  connect(m_redoAction, &QAction::triggered, this, &MainWindow::redo);

  m_cutAction = new QAction(QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
  m_cutAction->setShortcuts(QKeySequence::Cut);
  connect(m_cutAction, &QAction::triggered, this, &MainWindow::cutClip);

  m_copyAction = new QAction(QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
  m_copyAction->setShortcuts(QKeySequence::Copy);
  connect(m_copyAction, &QAction::triggered, this, &MainWindow::copyClip);

  m_pasteAction =
      new QAction(QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
  m_pasteAction->setShortcuts(QKeySequence::Paste);
  connect(m_pasteAction, &QAction::triggered, this, &MainWindow::pasteClip);

  m_deleteAction =
      new QAction(QIcon::fromTheme("edit-delete"), tr("&Delete"), this);
  m_deleteAction->setShortcut(QKeySequence::Delete);
  connect(m_deleteAction, &QAction::triggered, this, &MainWindow::deleteClip);

  m_splitAction = new QAction(tr("Split at Playhead"), this);
  m_splitAction->setShortcut(QKeySequence("Ctrl+B"));
  connect(m_splitAction, &QAction::triggered, this,
          &MainWindow::splitClipAtPlayhead);

  // View actions
  m_zoomInAction =
      new QAction(QIcon::fromTheme("zoom-in"), tr("Zoom &In"), this);
  m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
  connect(m_zoomInAction, &QAction::triggered, this, &MainWindow::zoomIn);

  m_zoomOutAction =
      new QAction(QIcon::fromTheme("zoom-out"), tr("Zoom &Out"), this);
  m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
  connect(m_zoomOutAction, &QAction::triggered, this, &MainWindow::zoomOut);

  m_fullscreenAction = new QAction(tr("F&ullscreen"), this);
  m_fullscreenAction->setShortcut(QKeySequence("F11"));
  m_fullscreenAction->setCheckable(true);
  connect(m_fullscreenAction, &QAction::triggered, this,
          &MainWindow::toggleFullscreen);

  // Edit mode actions
  m_editModeGroup = new QActionGroup(this);
  m_editModeGroup->setExclusive(true);

  m_selectModeAction =
      new QAction(QIcon::fromTheme("edit-select"), tr("Select"), this);
  m_selectModeAction->setCheckable(true);
  m_selectModeAction->setChecked(true);
  m_selectModeAction->setShortcut(QKeySequence("V"));
  m_editModeGroup->addAction(m_selectModeAction);
  connect(m_selectModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Select); });

  m_cutModeAction = new QAction(QIcon::fromTheme("edit-cut"), tr("Cut"), this);
  m_cutModeAction->setCheckable(true);
  m_cutModeAction->setShortcut(QKeySequence("C"));
  m_editModeGroup->addAction(m_cutModeAction);
  connect(m_cutModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Cut); });

  m_trimModeAction =
      new QAction(QIcon::fromTheme("edit-trim"), tr("Trim"), this);
  m_trimModeAction->setCheckable(true);
  m_trimModeAction->setShortcut(QKeySequence("T"));
  m_editModeGroup->addAction(m_trimModeAction);
  connect(m_trimModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Trim); });

  m_splitModeAction =
      new QAction(QIcon::fromTheme("edit-split"), tr("Split"), this);
  m_splitModeAction->setCheckable(true);
  m_splitModeAction->setShortcut(QKeySequence("S"));
  m_editModeGroup->addAction(m_splitModeAction);
  connect(m_splitModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Split); });

  m_handModeAction =
      new QAction(QIcon::fromTheme("tool-hand"), tr("Hand"), this);
  m_handModeAction->setCheckable(true);
  m_handModeAction->setShortcut(QKeySequence("H"));
  m_editModeGroup->addAction(m_handModeAction);
  connect(m_handModeAction, &QAction::triggered, this,
          [this]() { setEditMode(EditMode::Hand); });
}

void MainWindow::newProject() {
  if (!maybeSave())
    return;

  auto project = std::make_unique<ProjectFile>();
  project->addTrack(TrackType::Video, "Video 1");
  project->addTrack(TrackType::Audio, "Audio 1");
  setCurrentProject(std::move(project));
  m_currentFilePath.clear();
  setWindowTitle("CinemaStudio - Untitled Project");
}

void MainWindow::openProject() {
  if (!maybeSave())
    return;

  QString filePath = QFileDialog::getOpenFileName(
      this, "Open Project", QString(),
      "CinemaStudio Project (*.csp);;All Files (*)");

  if (filePath.isEmpty())
    return;

  auto project = std::make_unique<ProjectFile>();
  if (project->loadFromFile(filePath)) {
    setCurrentProject(std::move(project));
    m_currentFilePath = filePath;
    setWindowTitle(
        QString("CinemaStudio - %1").arg(QFileInfo(filePath).fileName()));
  } else {
    QMessageBox::critical(this, "Error", "Failed to open project file.");
  }
}

void MainWindow::saveProject() {
  if (m_currentFilePath.isEmpty()) {
    saveProjectAs();
    return;
  }

  if (m_currentProject->saveToFile(m_currentFilePath)) {
    statusBar()->showMessage("Project saved", 3000);
  } else {
    QMessageBox::critical(this, "Error", "Failed to save project.");
  }
}

void MainWindow::saveProjectAs() {
  QString filePath = QFileDialog::getSaveFileName(
      this, "Save Project", QString(),
      "CinemaStudio Project (*.csp);;All Files (*)");

  if (filePath.isEmpty())
    return;

  if (!filePath.endsWith(".csp")) {
    filePath += ".csp";
  }

  if (m_currentProject->saveToFile(filePath)) {
    m_currentFilePath = filePath;
    setWindowTitle(
        QString("CinemaStudio - %1").arg(QFileInfo(filePath).fileName()));
    statusBar()->showMessage("Project saved", 3000);
  } else {
    QMessageBox::critical(this, "Error", "Failed to save project.");
  }
}

void MainWindow::exportVideo() {
  if (!m_exportDialog) {
    m_exportDialog = new ExportDialog(this);
  }

  m_exportDialog->setProject(m_currentProject.get());
  m_exportDialog->exec();
}

void MainWindow::undo() {
  if (m_undoRedoManager) {
    m_undoRedoManager->undo();
  }
}

void MainWindow::redo() {
  if (m_undoRedoManager) {
    m_undoRedoManager->redo();
  }
}

void MainWindow::cutClip() {
  // TODO: Implement cut
}

void MainWindow::copyClip() {
  // TODO: Implement copy
}

void MainWindow::pasteClip() {
  // TODO: Implement paste
}

void MainWindow::deleteClip() {
  // TODO: Implement delete
}

void MainWindow::selectAll() {
  // TODO: Implement select all
}

void MainWindow::zoomIn() {
  if (m_timeline) {
    m_timeline->zoomIn();
  }
}

void MainWindow::zoomOut() {
  if (m_timeline) {
    m_timeline->zoomOut();
  }
}

void MainWindow::resetZoom() {
  if (m_timeline) {
    m_timeline->resetZoom();
  }
}

void MainWindow::toggleFullscreen() {
  if (isFullScreen()) {
    showNormal();
  } else {
    showFullScreen();
  }
}

void MainWindow::setEditMode(EditMode mode) {
  m_currentEditMode = mode;
  if (m_timeline) {
    m_timeline->setEditMode(mode);
  }
}

void MainWindow::splitClipAtPlayhead() {
  if (m_timeline && m_currentProject) {
    m_timeline->splitClipAtPlayhead();
  }
}

void MainWindow::trimClipStart() {
  // TODO: Implement
}

void MainWindow::trimClipEnd() {
  // TODO: Implement
}

void MainWindow::importMedia() {
  QStringList files = QFileDialog::getOpenFileNames(
      this, "Import Media", QString(),
      "Video Files (*.mp4 *.avi *.mov *.mkv *.wmv);;"
      "Audio Files (*.mp3 *.wav *.aac *.flac);;"
      "Image Files (*.png *.jpg *.jpeg *.bmp);;"
      "All Files (*)");

  if (!files.isEmpty()) {
    importMediaFiles(files);
  }
}

void MainWindow::importMediaFiles(const QStringList &files) {
  if (m_mediaLibrary) {
    m_mediaLibrary->addFiles(files);
  }
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (maybeSave()) {
    saveSettings();
    event->accept();
  } else {
    event->ignore();
  }
}

bool MainWindow::maybeSave() {
  if (!m_currentProject || !m_currentProject->isModified()) {
    return true;
  }

  QMessageBox::StandardButton ret = QMessageBox::warning(
      this, "CinemaStudio",
      "The project has been modified.\n"
      "Do you want to save your changes?",
      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

  if (ret == QMessageBox::Save) {
    saveProject();
    return true;
  } else if (ret == QMessageBox::Discard) {
    return true;
  }
  return false;
}

void MainWindow::setCurrentProject(std::unique_ptr<ProjectFile> project) {
  m_currentProject = std::move(project);

  if (m_videoPlayer) {
    m_videoPlayer->setProject(m_currentProject.get());
  }
  if (m_timeline) {
    m_timeline->setProject(m_currentProject.get());
  }
  if (m_audioMixer) {
    m_audioMixer->setProject(m_currentProject.get());
  }
}

void MainWindow::loadSettings() {
  QSettings settings("CinemaStudio", "CinemaStudio");
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::saveSettings() {
  QSettings settings("CinemaStudio", "CinemaStudio");
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());
}

} // namespace cinemastudio
