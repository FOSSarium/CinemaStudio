#include "MediaLibrary.h"
#include <QVBoxLayout>
#include <QListWidget>
namespace cinemastudio {
MediaLibrary::MediaLibrary(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_fileList = new QListWidget(this);
    layout->addWidget(m_fileList);
}
void MediaLibrary::addFiles(const QStringList& files) {
    m_fileList->addItems(files);
}
}
