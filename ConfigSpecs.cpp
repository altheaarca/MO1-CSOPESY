// for Configuration setting (config.txt)
#include "ConfigSpecs.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

ConfigSpecs::ConfigSpecs(const std::string& configFileName) {
    parseConfigFile(configFileName);
}

void ConfigSpecs::parseConfigFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "Failed to open config file: " << fileName << "\n";
        return;
    }

    std::string key;
    while (file >> key) {
        if (key == "num-cpu") {
            file >> numCPU;
        }
        else if (key == "scheduler") {
            file >> std::quoted(schedulerType);
        }
        else if (key == "quantum-cycles") {
            file >> quantumCycles;
        }
        else if (key == "batch-process-freq") {
            file >> batchProcessFreq;
        }
        else if (key == "min-ins") {
            file >> minInstructions;
        }
        else if (key == "max-ins") {
            file >> maxInstructions;
        }
        else if (key == "delay-per-exec") {
            file >> delayPerExecution;
        }

        // NEW parameters
        else if (key == "max-overall-mem") {
            file >> maxOverallMemory;
        }
        else if (key == "mem-per-frame") {
            file >> memPerFrame;
        }
        else if (key == "min-mem-per-proc") {
            file >> minMemPerProc;
        }
        else if (key == "max-mem-per-proc") {
            file >> maxMemPerProc;
        }
    }
}

// Getters
std::uint32_t ConfigSpecs::getNumCPU() const { return numCPU; }
std::string ConfigSpecs::getSchedulerType() const { return schedulerType; }
std::uint32_t ConfigSpecs::getQuantumCycles() const { return quantumCycles; }
std::uint32_t ConfigSpecs::getBatchProcessFreq() const { return batchProcessFreq; }
std::uint32_t ConfigSpecs::getMinInstructions() const { return minInstructions; }
std::uint32_t ConfigSpecs::getMaxInstructions() const { return maxInstructions; }
std::uint32_t ConfigSpecs::getDelayPerExecution() const { return delayPerExecution; }

std::uint32_t ConfigSpecs::getMaxOverallMem() const { return maxOverallMemory; }
std::uint32_t ConfigSpecs::getMemPerFrame() const { return memPerFrame; }
std::uint32_t ConfigSpecs::getMinMemPerProc() const { return minMemPerProc; }
std::uint32_t ConfigSpecs::getMaxMemPerProc() const { return maxMemPerProc; }

void ConfigSpecs::printConfig() const {
    std::cout << "=== ConfigSpecs ===\n"
        << "CPU Count:             " << numCPU << "\n"
        << "Scheduler Type:        " << schedulerType << "\n"
        << "Quantum Cycles:        " << quantumCycles << "\n"
        << "Batch Proc Freq:       " << batchProcessFreq << "\n"
        << "Min Instructions:      " << minInstructions << "\n"
        << "Max Instructions:      " << maxInstructions << "\n"
        << "Delay per Execution:   " << delayPerExecution << "\n"
        << "Max Overall Memory:    " << maxOverallMemory << "\n"
        << "Memory per Frame:      " << memPerFrame << "\n"
        << "Min Memory per Proc:   " << minMemPerProc << "\n"
        << "Max Memory per Proc:   " << maxMemPerProc << "\n\n";
}
