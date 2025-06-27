#pragma once
#include <memory>
#include "ConfigSpecs.h"
#include "ProcessManager.h"

class CPUScheduler {
public:
    explicit CPUScheduler(std::shared_ptr<ConfigSpecs> config)
        : config(config) {
    }
    void initializeAlgorithm();

    virtual ~CPUScheduler() = default;

    virtual void schedule() = 0;

protected:
    std::shared_ptr<ConfigSpecs> config;

    // Optional: provide getters so derived classes don't need to call config->...
    std::uint32_t getCPUCount() const { return config->getNumCPU(); }
    std::uint32_t getDelayPerExec() const { return config->getDelayPerExecution(); }
    std::uint32_t getQuantumCycles() const { return config->getQuantumCycles(); }
    std::string getSchedulerType() const { return config->getSchedulerType(); }
};
