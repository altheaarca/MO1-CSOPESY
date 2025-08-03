#include "CommandManager.h"
#include "OSController.h"
#include <random>


//TODO: random commands generation 
std::vector<std::shared_ptr<Command>> CommandManager::generateCommands() {
    uint32_t minIns = OSController::getInstance()->getConfig()->getMinInstructions();
    uint32_t maxIns = OSController::getInstance()->getConfig()->getMaxInstructions();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(minIns, maxIns);
    std::uniform_int_distribution<int> addVal(1, 10);  // for ADD constant

    uint32_t count = dist(gen);
    if (count % 2 != 0) count--; // Make it even to alternate PRINT/ADD properly

    std::vector<std::shared_ptr<Command>> commands;
    commands.reserve(count);

    for (uint32_t i = 0; i < count; i += 2) {
        // PRINT x
        commands.push_back(std::make_shared<Command>(Command::makePrint("x")));
        uint16_t val = addVal(gen);
        // ADD x, x, rand(1–10)
        commands.push_back(std::make_shared<Command>(Command::makeAdd("x", "x",val)));
    }

    return commands;
}

// TODO: commands specified by user. 