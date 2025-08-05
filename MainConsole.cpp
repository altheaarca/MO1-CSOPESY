#include "MainConsole.h"
#include "OSController.h"
#include "ProcessConsole.h"
#include "Process.h"
#include "Command.h"
#include <iostream>
#include <sstream>
#include <iomanip>  // for std::put_time
#include <ctime>    // for std::localtime, std::time_t

void MainConsole::runConsole() {
    auto consoleManager = OSController::getInstance()->getConsoleManager();
    auto commandManager = OSController::getInstance()->getCommandManager();
    auto processManager = OSController::getInstance()->getProcessManager();
    auto cpuScheduler = OSController::getInstance()->getCPUScheduler();
    auto memoryManager = OSController::getInstance()->getMemoryManager();

    while (true) {
        std::cout << "> ";
        std::string input;
        std::getline(std::cin, input);

        if (input.empty())
            continue;

        auto tokens = tokenizeInput(input);

        /*       for (std::size_t i = 0; i < tokens.size(); ++i) {
                   std::cout << "Token[" << i << "]: " << tokens[i] << std::endl;
               }*/

        if (tokens.size() == 1) {
            const std::string& cmd = tokens[0];
            if (cmd == "marquee") {
                std::cout << "Command recognized: marquee\n\n";
            }
            else if (cmd == "clear") {
                consoleManager->clearScreen();
            }
            else if (cmd == "report-util") {
                cpuScheduler->reportUtil();
                std::cout << "Report util: check csopesy-log.txt\n\n";
            }
            else if (cmd == "scheduler-start") {
                cpuScheduler->startSchedulingProcesses();
                std::cout << "Scheduler started.\n\n";
            }
            else if (cmd == "scheduler-stop") {
                cpuScheduler->stopSchedulingProcesses();
                std::cout << "Scheduler stopped.\n\n";
            }
            else if (cmd == "vmstat") {
                memoryManager->virtualMemoryStat();
            }
            else if (cmd == "process-smi") {
                memoryManager->processInformation();
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
                cpuScheduler->screenList();
            }
            else {
                std::cout << "Command not found.\n\n";
            }
        }
        else if (tokens.size() == 3) {
            std::string cmd3 = tokens[0] + " " + tokens[1];

            if (cmd3 == "screen -s") {
                if (consoleManager->processConsoleExists(tokens[2])) {
                    std::cout << "Process console '" << tokens[2] << "' already exists.\n\n";
                }
                else {
                    auto commands = commandManager->generateCommands();

                    int id = consoleManager->getGlobalProcessID();
                    consoleManager->incrementGlobalProcessID();

                    auto newProcess = processManager->createProcess(id, tokens[2], commands);
                    auto processConsole = std::make_shared<ProcessConsole>(tokens[2], newProcess);

                    consoleManager->createProcessConsole(tokens[2], processConsole);
                    cpuScheduler->addProcessToReadyQueue(newProcess);
                    consoleManager->switchToProcessConsole(tokens[2]);
                }
            }
            else if (cmd3 == "screen -r") {
                if (consoleManager->getProcessConsole(tokens[2])->getAttachedProcess()->isProcessStoppedDueToMemoryAccessError()) {
                    auto process = consoleManager->getProcessConsole(tokens[2])->getAttachedProcess();
                    time_t stopTime = process->getProcessStoppedOn();

                    std::tm timeInfo;
                    localtime_s(&timeInfo, &stopTime);
                    std::ostringstream oss;
                    oss << "" << std::put_time(&timeInfo, "%H:%M:%S") << "";
                    std::string formattedTime = oss.str();

                    std::cout << "Process " << process->getProcessName() << " shut down due to memory access violation error that occured at " <<
                        formattedTime << ". " << process->getInvalidMemoryAccess() << " invalid." << std::endl;
                }
                else if (consoleManager->getProcessConsole(tokens[2])->getAttachedProcess()->isFinished()) {
                    std::cout << "Process not found.\n\n";
                }
                else {
                    consoleManager->switchToProcessConsole(tokens[2]);
                }
            }
            else {
                std::cout << "Command not found.\n\n";
            }
        }
        else if (tokens.size() == 4) {
            std::string cmd3 = tokens[0] + " " + tokens[1];

            if (cmd3 == "screen -s") {
                if (consoleManager->processConsoleExists(tokens[2])) {
                    std::cout << "Process console '" << tokens[2] << "' already exists.\n\n";
                }
                else {
                    const std::vector<uint32_t> allowedSizes = {
                        64, 128, 256, 512, 1024, 2048, 4096,
                        8192, 16384, 32768, 65536
                    };

                    uint32_t memSize = 0;
                    try {
                        memSize = std::stoul(tokens[3]);
                    }
                    catch (...) {
                        std::cout << "[ERROR] Invalid memory size input.\n";
                        continue;
                    }

                    if (std::find(allowedSizes.begin(), allowedSizes.end(), memSize) == allowedSizes.end()) {
                        std::cout << "[ERROR] Invalid process memory size. Must be one of: ";
                        for (auto sz : allowedSizes) std::cout << sz << " ";
                        std::cout << '\n';
                        continue;
                    }

                    auto commands = commandManager->generateCommands();

                    int id = consoleManager->getGlobalProcessID();
                    consoleManager->incrementGlobalProcessID();

                    auto newProcess = processManager->createProcessWithMemorySize(id, tokens[2], memSize, commands);
                    auto processConsole = std::make_shared<ProcessConsole>(tokens[2], newProcess);

                    consoleManager->createProcessConsole(tokens[2], processConsole);
                    cpuScheduler->addProcessToReadyQueue(newProcess);
                    consoleManager->switchToProcessConsole(tokens[2]);
                }
            }
            else if (cmd3 == "screen -c") {
                if (consoleManager->processConsoleExists(tokens[2])) {
                    std::cout << "Process console '" << tokens[2] << "' already exists.\n\n";
                }
                else {
                    int id = consoleManager->getGlobalProcessID();
                    consoleManager->incrementGlobalProcessID();

                    auto newProcess = processManager->createProcessWithStringCommands(id, 
																									tokens[2], 
																									    tokens[3]);
                    auto processConsole = std::make_shared<ProcessConsole>(tokens[2], newProcess);
                    consoleManager->createProcessConsole(tokens[2], processConsole);
                    cpuScheduler->addProcessToReadyQueue(newProcess);
                    /*    consoleManager->switchToProcessConsole(tokens[2]);*/
                }
            }
        }
        else if (tokens.size() == 5) {
            std::string cmd3 = tokens[0] + " " + tokens[1];

            if (cmd3 == "screen -c") {
                if (consoleManager->processConsoleExists(tokens[2])) {
                    std::cout << "Process console '" << tokens[2] << "' already exists.\n\n";
                }
                else {
                    const std::vector<uint32_t> allowedSizes = {
                     64, 128, 256, 512, 1024, 2048, 4096,
                        8192, 16384, 32768, 65536
                    };

                    uint32_t memSize = 0;
                    try {
                        memSize = std::stoul(tokens[3]);
                    }
                    catch (...) {
                        std::cout << "[ERROR] Invalid memory size input.\n";
                        continue;
                    }

                    if (std::find(allowedSizes.begin(), allowedSizes.end(), memSize) == allowedSizes.end()) {
                        std::cout << "[ERROR] Invalid process memory size. Must be one of: ";
                        for (auto sz : allowedSizes) std::cout << sz << " ";
                        std::cout << '\n';
                        continue;
                    }

                    int id = consoleManager->getGlobalProcessID();
                    consoleManager->incrementGlobalProcessID();

                    auto newProcess = processManager->createProcessWithStringCommandsAndMemorySize(id, tokens[2], memSize, tokens[4]);
                    auto processConsole = std::make_shared<ProcessConsole>(tokens[2], newProcess);
                    consoleManager->createProcessConsole(tokens[2], processConsole);
                    cpuScheduler->addProcessToReadyQueue(newProcess);
                    consoleManager->switchToProcessConsole(tokens[2]);
                }
            }
        }
        else {
            std::cout << "Command not found.\n\n";
        }
    }
}

std::vector<std::string> MainConsole::tokenizeInput(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    // First, tokenize as usual
    while (iss >> token) {
        tokens.push_back(token);
    }

    // Find the index where the quoted string starts
    std::size_t quoteStartIndex = std::string::npos;
    for (std::size_t i = 0; i < tokens.size(); ++i) {
        if (!tokens[i].empty() && tokens[i].front() == '"') {
            quoteStartIndex = i;
            break;
        }
    }

    // Merge all tokens from quoteStartIndex onwards if found
    if (quoteStartIndex != std::string::npos) {
        std::string mergedToken = tokens[quoteStartIndex];
        for (std::size_t i = quoteStartIndex + 1; i < tokens.size(); ++i) {
            mergedToken += " " + tokens[i];
        }

        tokens.erase(tokens.begin() + quoteStartIndex, tokens.end());
        tokens.push_back(mergedToken);
    }

    return tokens;
}






