#include "HistoryManager.hpp"

HistoryManager::HistoryManager(size_t maxHistorySize)
    : _maxHistorySize(maxHistorySize) {}

HistoryManager::~HistoryManager()
{
    clear();
}

void HistoryManager::executeCommand(std::unique_ptr<ICommand> command)
{
    if (!command)
        return;

    command->execute();

    _undoStack.push_back(std::move(command));
    _trimHistory();
    _clearRedoStack();
}

bool HistoryManager::undo()
{
    if (!canUndo())
        return false;

    std::unique_ptr<ICommand> command = std::move(_undoStack.back());
    _undoStack.pop_back();

    command->undo();

    _redoStack.push_back(std::move(command));

    return true;
}

bool HistoryManager::redo()
{
    if (!canRedo())
        return false;

    std::unique_ptr<ICommand> command = std::move(_redoStack.back());
    _redoStack.pop_back();

    command->execute();

    _undoStack.push_back(std::move(command));

    return true;
}

bool HistoryManager::canUndo() const
{
    return !_undoStack.empty();
}

bool HistoryManager::canRedo() const
{
    return !_redoStack.empty();
}

void HistoryManager::clear()
{
    _undoStack.clear();
    _redoStack.clear();
}

void HistoryManager::setMaxHistorySize(size_t size)
{
    _maxHistorySize = size;
    _trimHistory();
}

size_t HistoryManager::getMaxHistorySize() const
{
    return _maxHistorySize;
}

void HistoryManager::_trimHistory()
{
    while (_undoStack.size() > _maxHistorySize) {
        _undoStack.erase(_undoStack.begin());
    }
}

void HistoryManager::_clearRedoStack()
{
    _redoStack.clear();
}