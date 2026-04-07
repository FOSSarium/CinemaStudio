#pragma once
#include <QDialog>
namespace cinemastudio { class ProjectFile;
class ExportDialog : public QDialog { 
public: ExportDialog(QWidget* parent = nullptr);
void setProject(ProjectFile* project); }; }
