#include "OSController.h"
#include "ConfigSpecs.h"
#include "ConsoleManager.h"
#include "MainConsole.h"
#include "CommandManager.h"
#include "ProcessManager.h"
#include "CPUScheduler.h"

int main() {
    auto consoleManager = std::make_shared<ConsoleManager>();
    auto commandManager = std::make_shared<CommandManager>();
    auto processManager = std::make_shared<ProcessManager>();

    OSController::getInstance()->setConsoleManager(consoleManager);
    OSController::getInstance()->getConsoleManager()->CSOPESYHeader();

    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);

        if (!OSController::getInstance()->isOSInitialized()) {
            if (input == "in" || input == "initialize") {
                auto config = std::make_shared<ConfigSpecs>("config.txt");
                auto memoryManager = std::make_shared<MemoryManager>(config);// directly
                auto scheduler = std::make_shared<CPUScheduler>(config, memoryManager);

                OSController::getInstance()->injectCoreComponents(config, commandManager, processManager);
                OSController::getInstance()->initialize();
                OSController::getInstance()->setCPUScheduler(scheduler);

                break;
            }
            else {
                std::cout << "Please initialize." << std::endl << std::endl;
            }
        }
    }

    OSController::getInstance()->getConsoleManager()->clearScreen();
    MainConsole ad;
    ad.runConsole();
    return 0;
}
