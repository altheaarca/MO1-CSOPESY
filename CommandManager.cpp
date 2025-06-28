#include "CommandManager.h"
#include "OSController.h"
#include <random>

std::vector<std::shared_ptr<Command>> CommandManager::generateCommands() {
    uint32_t minIns = OSController::getInstance()->getConfig()->getMinInstructions();
    uint32_t maxIns = OSController::getInstance()->getConfig()->getMaxInstructions();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(minIns, maxIns);
    std::uniform_int_distribution<int> typePicker(0, 0); // For now only PRINT

    uint32_t count = dist(gen);
    std::vector<std::shared_ptr<Command>> commands;
    commands.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        Command::CommandType type = static_cast<Command::CommandType>(typePicker(gen));
        commands.push_back(std::make_shared<Command>(type));
    }

    return commands;
}
