#include "UndoRedoManager.h"
namespace cinemastudio {
UndoRedoManager::UndoRedoManager(QObject* parent) : QObject(parent) {}
void UndoRedoManager::undo() { if (!m_undoStack.isEmpty()) { auto action = m_undoStack.pop(); m_redoStack.push(action); action(); } }
void UndoRedoManager::redo() { if (!m_redoStack.isEmpty()) { auto action = m_redoStack.pop(); m_undoStack.push(action); action(); } }
void UndoRedoManager::push(std::function<void()> action) { m_undoStack.push(action); m_redoStack.clear(); }
}
