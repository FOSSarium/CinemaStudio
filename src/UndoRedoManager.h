#pragma once
#include <QObject>
#include <QStack>
#include <functional>
namespace cinemastudio {
class UndoRedoManager : public QObject {
public:
    explicit UndoRedoManager(QObject* parent = nullptr) : QObject(parent) {}
    void undo() { if (!m_undoStack.isEmpty()) { auto a = m_undoStack.pop(); m_redoStack.push(a); a(); } }
    void redo() { if (!m_redoStack.isEmpty()) { auto a = m_redoStack.pop(); m_undoStack.push(a); a(); } }
    void push(std::function<void()> action) { m_undoStack.push(action); m_redoStack.clear(); }
private:
    QStack<std::function<void()>> m_undoStack;
    QStack<std::function<void()>> m_redoStack;
}; }
