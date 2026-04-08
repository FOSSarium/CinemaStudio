#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

namespace cinemastudio {

class MediaLibrary : public QWidget {
  Q_OBJECT
public:
  explicit MediaLibrary(QWidget *parent = nullptr);
  void addFiles(const QStringList &files);
  QStringList getSelectedFiles() const;

private:
  void setupUI();
  void onImportClicked();
  void onSearchChanged(const QString &text);

  QLineEdit *m_searchBox = nullptr;
  QListWidget *m_fileList = nullptr;
  QPushButton *m_importBtn = nullptr;
  QLabel *m_countLabel = nullptr;
  int m_fileCount = 0;
};

} // namespace cinemastudio
