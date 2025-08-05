#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Process.h"
#include "Command.h"
#include "MemoryManager.h"

class ProcessManager {
public:
    std::shared_ptr<Process> createProcess(int id, const std::string& name, const std::vector<std::shared_ptr<Command>> commands);
    std::shared_ptr<Process> createProcessWithMemorySize(int id, const std::string& name, std::uint32_t processSize, const std::vector<std::shared_ptr<Command>> commands);
    std::shared_ptr<Process> createProcessWithStringCommands(int id, const std::string& name, std::string commands);
    std::shared_ptr<Process> createProcessWithStringCommandsAndMemorySize(int id, const std::string& name, std::uint32_t processSize, std::string commands);
};
