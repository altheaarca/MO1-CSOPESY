// ProcessManager.cpp
#include "ProcessManager.h"
#include <iostream>
#include <cmath>

std::shared_ptr<Process> ProcessManager::createProcess(int id,
    const std::string& name,
    const std::vector<std::shared_ptr<Command>>& commands,
    uint32_t memorySize,
    uint32_t frameSize,
    std::shared_ptr<MemoryManager> memoryManager)
{
    if (!memoryManager || !memoryManager->allocateMemory(name)) {
        std::cout << "[ProcessManager] Memory allocation failed for '" << name << "'\n";
        return nullptr;
    }

    auto proc = std::make_shared<Process>(id, name, commands);
    uint32_t pageCount = (uint32_t)std::ceil((double)memorySize / frameSize);
    proc->pageTable.resize(pageCount);
    for (uint32_t i = 0; i < pageCount; ++i) {
        proc->pageTable[i] = { (int)i, -1, false, false, 0 };
    }
    proc->pageCount = pageCount;
    std::cout << "[ProcessManager] '" << name << "' has " << pageCount << " pages\n";
    return proc;
}
