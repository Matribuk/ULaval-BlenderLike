#pragma once

#include <string>
#include <memory>

class ICommand {
    public:
        virtual ~ICommand() = default;
        virtual void execute() = 0;
        virtual void undo() = 0;
        virtual void redo() = 0;
        virtual std::string getDescription() const = 0;
};
