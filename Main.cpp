#include "OSController.h"
#include "ConfigSpecs.h"
#include "ConsoleManager.h"
#include "MainConsole.h"
#include "CommandManager.h"
#include "ProcessManager.h"
#include "CPUScheduler.h"
#include "MemoryManager.h"
#include "BackingStore.h"

int main() {
    auto consoleManager = std::make_shared<ConsoleManager>();
    auto commandManager = std::make_shared<CommandManager>();
    auto processManager = std::make_shared<ProcessManager>();
    auto backingStore = std::make_shared<BackingStore>("csopesy-backing-store.txt", "backing-store-operation-logs.txt");

    OSController::getInstance()->setConsoleManager(consoleManager);
    OSController::getInstance()->getConsoleManager()->CSOPESYHeader();

    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);

        if (!OSController::getInstance()->isOSInitialized()) {
            if (input == "in" || input == "initialize") {

                auto config = std::make_shared<ConfigSpecs>("config.txt");

                auto memoryManager = std::make_shared<MemoryManager>(config->getMaxOverallMem(), config->getMemPerFrame());

                auto cpuScheduler = std::make_shared<CPUScheduler>();

                OSController::getInstance()->injectCoreComponents(config, commandManager, processManager, cpuScheduler, memoryManager, backingStore);

                cpuScheduler->runScheduler();

                OSController::getInstance()->initialize();

                std::cout << "Intialized." << std::endl;

                consoleManager->clearScreen();
                break;
            }
            else if (input == "exit") {
                exit(0);
            }
            else {
                std::cout << "Please initialize." << std::endl << std::endl;
            }
        }
    }

    MainConsole ad;
    ad.runConsole();

    return 0;
}
