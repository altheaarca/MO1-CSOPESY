#include "ProcessManager.h"

std::shared_ptr<Process> ProcessManager::createProcess(int id, const std::string& name, const std::vector<std::shared_ptr<Command>> commands)
{
    return std::make_shared<Process>(id, name, commands);
}

std::shared_ptr<Process> ProcessManager::createProcessWithMemorySize(int id, const std::string& name, std::uint32_t processSize, const std::vector<std::shared_ptr<Command>> commands)
{
    return std::make_shared<Process>(id, name, processSize, commands);
}

std::shared_ptr<Process> ProcessManager::createProcessWithStringCommands(int id, const std::string& name, std::string commands)
{
    return std::make_shared<Process>(id, name, commands);
}

std::shared_ptr<Process> ProcessManager::createProcessWithStringCommandsAndMemorySize(int id, const std::string& name, std::uint32_t processSize, std::string commands)
{
    return std::make_shared<Process>(id, name, processSize, commands);
}
