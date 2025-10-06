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

    this->_undoStack.push_back(std::move(command));
    this->_trimHistory();
    this->_clearRedoStack();
}

bool HistoryManager::undo()
{
    if (!canUndo())
        return false;

    std::unique_ptr<ICommand> command = std::move(this->_undoStack.back());
    this->_undoStack.pop_back();

    command->undo();

    this->_redoStack.push_back(std::move(command));

    return true;
}

bool HistoryManager::redo()
{
    if (!canRedo())
        return false;

    std::unique_ptr<ICommand> command = std::move(this->_redoStack.back());
    this->_redoStack.pop_back();

    command->execute();

    this->_undoStack.push_back(std::move(command));

    return true;
}

bool HistoryManager::canUndo() const
{
    return !this->_undoStack.empty();
}

bool HistoryManager::canRedo() const
{
    return !this->_redoStack.empty();
}

void HistoryManager::clear()
{
    this->_undoStack.clear();
    this->_redoStack.clear();
}

void HistoryManager::setMaxHistorySize(size_t size)
{
    this->_maxHistorySize = size;
    this->_trimHistory();
}

size_t HistoryManager::getMaxHistorySize() const
{
    return this->_maxHistorySize;
}

void HistoryManager::_trimHistory()
{
    while (this->_undoStack.size() > this->_maxHistorySize) {
        this->_undoStack.erase(this->_undoStack.begin());
    }
}

void HistoryManager::_clearRedoStack()
{
    this->_redoStack.clear();
}