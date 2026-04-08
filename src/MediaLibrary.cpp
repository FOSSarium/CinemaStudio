#include "MediaLibrary.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>

namespace cinemastudio {

MediaLibrary::MediaLibrary(QWidget *parent) : QWidget(parent) { setupUI(); }

void MediaLibrary::setupUI() {
  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(8, 8, 8, 8);
  layout->setSpacing(8);

  // Search box
  m_searchBox = new QLineEdit(this);
  m_searchBox->setPlaceholderText("🔍 Search media...");
  m_searchBox->setClearButtonEnabled(true);
  connect(m_searchBox, &QLineEdit::textChanged, this,
          &MediaLibrary::onSearchChanged);
  layout->addWidget(m_searchBox);

  // Import button
  m_importBtn = new QPushButton("📁 Import Media", this);
  m_importBtn->setProperty("primary", true);
  m_importBtn->setCursor(Qt::PointingHandCursor);
  connect(m_importBtn, &QPushButton::clicked, this,
          &MediaLibrary::onImportClicked);
  layout->addWidget(m_importBtn);

  // File list
  m_fileList = new QListWidget(this);
  m_fileList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  m_fileList->setViewMode(QListView::ListMode);
  m_fileList->setSpacing(2);
  layout->addWidget(m_fileList, 1);

  // Count label
  m_countLabel = new QLabel("0 files", this);
  m_countLabel->setAlignment(Qt::AlignCenter);
  m_countLabel->setStyleSheet("color: #888888; font-size: 11px;");
  layout->addWidget(m_countLabel);
}

void MediaLibrary::addFiles(const QStringList &files) {
  for (const auto &file : files) {
    QFileInfo fi(file);
    auto *item = new QListWidgetItem(fi.fileName());
    item->setData(Qt::UserRole, file);
    item->setToolTip(file);

    // Set icon based on file type
    QString ext = fi.suffix().toLower();
    QString type;
    if (ext == "mp4" || ext == "avi" || ext == "mov" || ext == "mkv")
      type = "🎬 ";
    else if (ext == "mp3" || ext == "wav" || ext == "aac" || ext == "flac")
      type = "🎵 ";
    else if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp")
      type = "🖼️ ";
    else
      type = "📄 ";

    item->setText(type + fi.fileName());
    m_fileList->addItem(item);
  }
  m_fileCount = m_fileList->count();
  m_countLabel->setText(QString::number(m_fileCount) + " files");
}

void MediaLibrary::onImportClicked() {
  QStringList files = QFileDialog::getOpenFileNames(
      this, "Import Media", QString(),
      "Video Files (*.mp4 *.avi *.mov *.mkv *.wmv);;"
      "Audio Files (*.mp3 *.wav *.aac *.flac);;"
      "Image Files (*.png *.jpg *.jpeg *.bmp);;"
      "All Files (*)");
  if (!files.isEmpty())
    addFiles(files);
}

void MediaLibrary::onSearchChanged(const QString &text) {
  for (int i = 0; i < m_fileList->count(); i++) {
    auto *item = m_fileList->item(i);
    item->setHidden(!item->text().contains(text, Qt::CaseInsensitive));
  }
}

QStringList MediaLibrary::getSelectedFiles() const {
  QStringList result;
  for (auto *item : m_fileList->selectedItems()) {
    result << item->data(Qt::UserRole).toString();
  }
  return result;
}

} // namespace cinemastudio
