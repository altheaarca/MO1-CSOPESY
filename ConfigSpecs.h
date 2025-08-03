#pragma once

#include <string>
#include <cstdint>

class ConfigSpecs {
public:
    explicit ConfigSpecs(const std::string& configFileName);

    std::uint32_t getNumCPU() const;
    std::string getSchedulerType() const;
    std::uint32_t getQuantumCycles() const;
    std::uint32_t getBatchProcessFreq() const;
    std::uint32_t getMinInstructions() const;
    std::uint32_t getMaxInstructions() const;
    std::uint32_t getDelayPerExecution() const;

    std::uint32_t getMaxOverallMemory() const;
    std::uint32_t getMemPerFrame() const;
    std::uint32_t getMemPerProcess() const;

    void printConfig() const;

private:
    void parseConfigFile(const std::string& fileName);

    std::uint32_t numCPU = 1;
    std::string schedulerType = "fcfs";
    std::uint32_t quantumCycles = 1;
    std::uint32_t batchProcessFreq = 1;
    std::uint32_t minInstructions = 1000;
    std::uint32_t maxInstructions = 2000;
    std::uint32_t delayPerExecution = 0;
    std::uint32_t maxOverallMemory = 16384;
    std::uint32_t memPerFrame = 16;
    std::uint32_t memPerProcess = 4096;
};

