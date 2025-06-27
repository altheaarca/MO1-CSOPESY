#include "MainConsole.h"
#include "OSController.h"
#include "ProcessConsole.h"
#include "Process.h"
#include "Command.h"
#include <iostream>
#include <sstream>
#include <limits>

#include "../MO1 - CSOPESY/customizedLayout.h"

void MainConsole::runConsole() {
    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);

        if (input.empty())
            continue;

        auto tokens = tokenizeInput(input);

        if (tokens.size() == 1) {
            const std::string& cmd = tokens[0];

            if (cmd == "a") {
                OSController::getInstance()->getConsoleManager()->listProcessConsoles();
            }
            else if (cmd == "nvidia-smi")
            {
                auto nvidiasmi = std::make_shared<customizedLayout>("nvidia-dummy");
                OSController::getInstance()->getConsoleManager()->createProcessConsole("nvidia-dummy", nvidiasmi);
                OSController::getInstance()->getConsoleManager()->switchToProcessConsole("nvidia-dummy");
            }
            else if (cmd == "initialize") {
                std::cout << "Command recognized: \n\n";
            }
            else if (cmd == "marquee") {
                std::cout << "Command recognized: marquee\n\n";
            }
            else if (cmd == "clear") {
                OSController::getInstance()->getConsoleManager()->clearScreen();
                std::cout << "Command recognized: clear\n\n";
            }
            else if (cmd == "report-util") {
                std::cout << "Command recognized: report-util\n\n";
            }
            else if (cmd == "scheduler-start") {
                std::cout << "Command recognized: scheduler-start\n\n";
            }
            else if (cmd == "scheduler-stop") {
                std::cout << "Command recognized: scheduler-stop\n\n";
            }
            else if (cmd == "exit") {
                exit(0);
            }

            else {
                std::cout << "Command not found.\n\n";
            }
        }
        else if (tokens.size() == 2) {
            std::string cmd2 = tokens[0] + " " + tokens[1];

            if (cmd2 == "screen -ls") {
                std::cout << "Command recognized: screen -ls\n\n";
            }
            else {
                std::cout << "Command not found.\n\n";
            }
        }
        else if (tokens.size() == 3) {
            std::string cmd3 = tokens[0] + " " + tokens[1];

            if (cmd3 == "screen -s") {
                if (OSController::getInstance()->getConsoleManager()->processConsoleExists(tokens[2])) {
                    std::cout << "Process console '" << tokens[2] << "' already exists.\n\n";
                }
                else {
                    auto c = OSController::getInstance()->getCommandManager()->generateCommands();

                    uint32_t minIns = OSController::getInstance()->getConfig()->getMinInstructions();
                    uint32_t maxIns = OSController::getInstance()->getConfig()->getMaxInstructions();

                    int id = OSController::getInstance()->getConsoleManager()->getGlobalProcessID();
                    OSController::getInstance()->getConsoleManager()->incrementGlobalProcessID();


                    auto newProcess = OSController::getInstance()->getProcessManager()->createProcess(id, tokens[2], c);
                    auto processConsole = std::make_shared<ProcessConsole>(tokens[2], newProcess);

                    OSController::getInstance()->getConsoleManager()->createProcessConsole(tokens[2], processConsole);
                    OSController::getInstance()->getConsoleManager()->switchToProcessConsole(tokens[2]);
                }
            }
            else if (cmd3 == "screen -r") {
                OSController::getInstance()->getConsoleManager()->switchToProcessConsole(tokens[2]);
            }
            else {
                std::cout << "Command not found.\n\n";
            }
        }
    }
}

std::vector<std::string> MainConsole::tokenizeInput(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}