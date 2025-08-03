#include "MainConsole.h"
#include "OSController.h"
#include "ProcessConsole.h"
#include "Process.h"
#include "Command.h"
#include <iostream>
#include <sstream>
#include <limits>
#include "customizedLayout.h"

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

			 if (cmd == "screen -ls") {
                 OSController::getInstance()->getConsoleManager()->listProcessConsoles();
            }
            else if (cmd == "clear") {
                OSController::getInstance()->getConsoleManager()->clearScreen();
                std::cout << "Command recognized: clear\n\n";
            }
            else if (cmd == "report-util") {
                 auto scheduler = OSController::getInstance()->getCPUScheduler();
                 scheduler->printUtil();
             }
            else if (cmd == "scheduler-start") {
                 auto scheduler = OSController::getInstance()->getCPUScheduler();

                 for (int i = 0; i < 10; ++i) {
                     auto commands = OSController::getInstance()->getCommandManager()->generateCommands();
                     auto proc = std::make_shared<Process>(i, "process" + std::to_string(i + 1), commands);
                     // Console setup
                     OSController::getInstance()->getConsoleManager()->createProcessConsole(
                         proc->getProcessName(),
                         std::make_shared<ProcessConsole>(proc->getProcessName(), proc)
                     );
                     if (proc != nullptr)
                         scheduler->addProcess(proc);
                 }
                 scheduler->start();
             }


            else if (cmd == "scheduler-stop") {
                std::cout << "Command recognized: scheduler-stop\n\n";

                auto scheduler = OSController::getInstance()->getCPUScheduler();
                if (scheduler) {
                    scheduler->stop();
                }
                else {
                    std::cout << "[Scheduler] No scheduler instance available.\n";
                }
            }
            else if (cmd == "process-smi") {
				 auto memory = OSController::getInstance()->getCPUScheduler()->getMemoryManager(); 
            	 memory->printMemoryStatus(); 
             }
            else if (cmd == "vmstat") {
                 auto scheduler = OSController::getInstance()->getCPUScheduler();
                 scheduler->printVMStat();
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
                OSController::getInstance()->getConsoleManager()->listProcessConsoles();
            }
        }

        else if (tokens.size() == 3) {
            std::string cmd3 = tokens[0] + " " + tokens[1];

            if (cmd3 == "screen -s") {
                const std::string& name = tokens[2];
                std::cout << "Missing memory size argument for screen -s.\n\n";
            }

            else if (cmd3 == "screen -r") {
                OSController::getInstance()->getConsoleManager()->switchToProcessConsole(tokens[2]);
            }
            else {
                std::cout << "Command not found.\n\n";
            }
        }
        else if (tokens.size() == 4) {
            std::string cmd3 = tokens[0] + " " + tokens[1];
            const std::string& name = tokens[2];
            const std::string& memStr = tokens[3];

            if (cmd3 == "screen -s") {
                int memSize = std::stoi(memStr);

                auto isPowerOf2 = [](int x) { return x >= 64 && x <= 65536 && (x & (x - 1)) == 0; };
                if (!isPowerOf2(memSize)) {
                    std::cout << "Invalid memory allocation. Must be power of 2 and between 64 and 65536.\n\n";
                    return;
                }

                if (OSController::getInstance()->getConsoleManager()->processConsoleExists(name)) {
                    std::cout << "Process console '" << name << "' already exists.\n\n";
                }
                else {
                    auto commands = OSController::getInstance()->getCommandManager()->generateCommands();

                    int id = OSController::getInstance()->getConsoleManager()->getGlobalProcessID();
                    OSController::getInstance()->getConsoleManager()->incrementGlobalProcessID();

                    auto scheduler = OSController::getInstance()->getCPUScheduler();
                    auto memoryManager = scheduler->getMemoryManager();
                    uint32_t frameSize = OSController::getInstance()->getConfig()->getMemPerFrame();
                    auto newProcess = OSController::getInstance()->getProcessManager()->createProcess(id, name, commands, memSize, frameSize, memoryManager);

                    if (!newProcess) {
                        std::cout << "Failed to create process '" << name << "' due to memory issues.\n\n";
                        return;
                    }

                    auto processConsole = std::make_shared<ProcessConsole>(name, newProcess);
                    OSController::getInstance()->getConsoleManager()->createProcessConsole(name, processConsole);
                    OSController::getInstance()->getCPUScheduler()->addProcess(newProcess);

                    std::cout << "Process '" << name << "' created with " << memSize << " bytes.\n\n";
                }
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