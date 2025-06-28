#include "ProcessManager.h"

std::shared_ptr<Process> ProcessManager::createProcess(int id, const std::string& name, const std::vector<std::shared_ptr<Command>>& commands) {
    return std::make_shared<Process>(id, name, commands);
}
