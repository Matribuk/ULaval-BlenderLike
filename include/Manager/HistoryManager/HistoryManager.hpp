#pragma once

#include "Manager/ICommand.hpp"
#include <vector>
#include <memory>

class HistoryManager {
    public:
        HistoryManager(size_t maxHistorySize = 100);
        ~HistoryManager();

        void executeCommand(std::unique_ptr<ICommand> command);
        bool undo();
        bool redo();
        bool canUndo() const;
        bool canRedo() const;
        void clear();

        void setMaxHistorySize(size_t size);
        size_t getMaxHistorySize() const;

    private:
        std::vector<std::unique_ptr<ICommand>> _undoStack;
        std::vector<std::unique_ptr<ICommand>> _redoStack;
        size_t _maxHistorySize;

        void _trimHistory();
        void _clearRedoStack();
};