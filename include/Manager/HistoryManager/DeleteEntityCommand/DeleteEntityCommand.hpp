#pragma once

#include "Manager/ICommand.hpp"

class DeleteEntityCommand : public ICommand {
    public:
        DeleteEntityCommand() = default;
        ~DeleteEntityCommand() = default;

    private:
};
