#pragma once
#include <QObject>
#include <QStack>
#include <functional>
namespace cinemastudio {
class UndoRedoManager : public QObject { 
public: UndoRedoManager(QObject* parent = nullptr);
void undo(); void redo();
void push(std::function<void()> action);
private: QStack<std::function<void()>> m_undoStack; QStack<std::function<void()>> m_redoStack; }; }
