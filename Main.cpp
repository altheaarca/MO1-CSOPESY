#include "OSController.h"
#include "ConfigSpecs.h"
#include "ConsoleManager.h"
#include "MainConsole.h"
#include "CommandManager.h"
#include "ProcessManager.h"
#include "Main.h"

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
                OSController::getInstance()->injectCoreComponents(config, commandManager, processManager);
                OSController::getInstance()->initialize();
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
