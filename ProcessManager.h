#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Process.h"
#include "Command.h"
#include "MemoryManager.h"

class ProcessManager {
public:
    std::shared_ptr<Process> createProcess(int id,
        const std::string& name,
        const std::vector<std::shared_ptr<Command>>& commands,
        uint32_t memorySize,
        uint32_t frameSize,
        std::shared_ptr<MemoryManager> memoryManager);
};
