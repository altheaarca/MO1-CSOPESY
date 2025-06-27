#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Process.h"
#include "Command.h"

class ProcessManager {
public:
    std::shared_ptr<Process> createProcess(int id, const std::string& name, const std::vector<std::shared_ptr<Command>>& commands);
};
