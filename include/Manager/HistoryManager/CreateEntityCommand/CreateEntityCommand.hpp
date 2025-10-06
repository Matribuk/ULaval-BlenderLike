#pragma once

#include "Manager/ICommand.hpp"

class CreateEntityCommand : public ICommand {
    public:
        CreateEntityCommand() = default;
        ~CreateEntityCommand() = default;

    private:
};
