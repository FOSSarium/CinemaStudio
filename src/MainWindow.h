#pragma once

#include "CommonTypes.h"
#include <QActionGroup>
#include <QDockWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QToolBar>
#include <memory>

class QCloseEvent;

namespace cinemastudio {

class ProjectFile;
class VideoPlayer;
class Timeline;
class MediaLibrary;
class Toolbar;
class EffectsPanel;
class AudioMixer;
class FilterPanel;
class PropertiesPanel;
class ExportDialog;
class UndoRedoManager;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  // File menu
  void newProject();
  void openProject();
  void saveProject();
  void saveProjectAs();
  void exportVideo();

  // Edit menu
  void undo();
  void redo();
  void cutClip();
  void copyClip();
  void pasteClip();
  void deleteClip();
  void selectAll();

  // View menu
  void zoomIn();
  void zoomOut();
  void resetZoom();
  void toggleFullscreen();

  // Tools
  void setEditMode(EditMode mode);
  void splitClipAtPlayhead();
  void trimClipStart();
  void trimClipEnd();

  // Media
  void importMedia();
  void importMediaFiles(const QStringList &files);

private:
  void setupUI();
  void setupMenus();
  void setupToolbar();
  void setupDockWidgets();
  void setupStatusBar();
  void createActions();
  void loadSettings();
  void saveSettings();
  bool maybeSave();
  void setCurrentProject(std::unique_ptr<ProjectFile> project);

  // UI Components
  VideoPlayer *m_videoPlayer = nullptr;
  Timeline *m_timeline = nullptr;
  MediaLibrary *m_mediaLibrary = nullptr;
  Toolbar *m_toolbar = nullptr;
  EffectsPanel *m_effectsPanel = nullptr;
  AudioMixer *m_audioMixer = nullptr;
  FilterPanel *m_filterPanel = nullptr;
  PropertiesPanel *m_propertiesPanel = nullptr;
  ExportDialog *m_exportDialog = nullptr;
  UndoRedoManager *m_undoRedoManager = nullptr;

  // Menus
  QMenu *m_fileMenu = nullptr;
  QMenu *m_editMenu = nullptr;
  QMenu *m_viewMenu = nullptr;
  QMenu *m_toolsMenu = nullptr;
  QMenu *m_helpMenu = nullptr;

  // Actions
  QAction *m_newAction = nullptr;
  QAction *m_openAction = nullptr;
  QAction *m_saveAction = nullptr;
  QAction *m_saveAsAction = nullptr;
  QAction *m_exportAction = nullptr;
  QAction *m_undoAction = nullptr;
  QAction *m_redoAction = nullptr;
  QAction *m_cutAction = nullptr;
  QAction *m_copyAction = nullptr;
  QAction *m_pasteAction = nullptr;
  QAction *m_deleteAction = nullptr;
  QAction *m_splitAction = nullptr;
  QAction *m_importAction = nullptr;
  QAction *m_zoomInAction = nullptr;
  QAction *m_zoomOutAction = nullptr;
  QAction *m_fullscreenAction = nullptr;

  QActionGroup *m_editModeGroup = nullptr;
  QAction *m_selectModeAction = nullptr;
  QAction *m_cutModeAction = nullptr;
  QAction *m_trimModeAction = nullptr;
  QAction *m_splitModeAction = nullptr;
  QAction *m_handModeAction = nullptr;

  // Project
  std::unique_ptr<ProjectFile> m_currentProject;
  QString m_currentFilePath;
  bool m_projectModified = false;

  // State
  EditMode m_currentEditMode = EditMode::Select;
  bool m_isFullscreen = false;
};

} // namespace cinemastudio
