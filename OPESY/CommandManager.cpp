//Handling the Barebones process instructions
#include "CommandManager.h"
#include "OSController.h"
#include <random>

std::vector<std::shared_ptr<Command>> CommandManager::generateCommands() {
    uint32_t minIns = OSController::getInstance()->getConfig()->getMinInstructions();
    uint32_t maxIns = OSController::getInstance()->getConfig()->getMaxInstructions();
    
    std::random_device rd;
    std::mt19937 gen(rd()); //  Mersenne Twister pseudo-random number generator.
    std::uniform_int_distribution<uint32_t> dist(minIns, maxIns);

    uint32_t count = dist(gen);//ensures the random number falls within minIns and maxIns.

    std::vector<std::shared_ptr<Command>> commands;
    commands.reserve(count);

    for (uint32_t i = 0; i < count; ++i) {
        commands.push_back(std::make_shared<Command>());  
    }

    return commands;
}
