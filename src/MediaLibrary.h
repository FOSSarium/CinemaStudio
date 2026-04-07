#pragma once
#include <QWidget>
#include <QListWidget>
#include <QStringList>
namespace cinemastudio {
class MediaLibrary : public QWidget { 
public: MediaLibrary(QWidget* parent = nullptr);
void addFiles(const QStringList& files);
private: QListWidget* m_fileList; }; }
