#pragma once

#include <vector>
#include <memory>
#include "Command.h"

class CommandManager {
public:
    std::vector<std::shared_ptr<Command>> generateCommands();
};


