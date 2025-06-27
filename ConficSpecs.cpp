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

void ConfigSpecs::printConfig() const {
    std::cout << "=== ConfigSpecs ===\n"
        << "CPU Count:           " << numCPU << "\n"
        << "Scheduler Type:      " << schedulerType << "\n"
        << "Quantum Cycles:      " << quantumCycles << "\n"
        << "Batch Proc Freq:     " << batchProcessFreq << "\n"
        << "Min Instructions:    " << minInstructions << "\n"
        << "Max Instructions:    " << maxInstructions << "\n"
        << "Delay per Execution: " << delayPerExecution << "\n";
}



