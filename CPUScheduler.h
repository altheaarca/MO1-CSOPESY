// CPUScheduler.h
#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <iostream>

#include "Process.h"
#include "ConfigSpecs.h"
#include "MemoryManager.h"

class CPUScheduler {
public:
    CPUScheduler(std::shared_ptr<ConfigSpecs> config, std::shared_ptr<MemoryManager> memManager);

    void start();
    void stop();
    void schedule();
    void addProcess(std::shared_ptr<Process> process);
    void printReport(std::ostream& out);
    void printUtil();
    void printVMStat();

    std::shared_ptr<MemoryManager> getMemoryManager() { return memoryManager; }

private:
    void schedulerThreadFunction();
    void runFCFS(uint32_t cpuId, std::shared_ptr<Process> proc);
    void runRR(uint32_t cpuId, std::shared_ptr<Process> proc);
    void moveToFinished(std::shared_ptr<Process> proc, uint32_t cpuId);

    std::shared_ptr<ConfigSpecs> config;
    std::shared_ptr<MemoryManager> memoryManager;

    std::queue<std::shared_ptr<Process>> readyQueue;
    std::vector<std::shared_ptr<Process>> runningProcesses;
    std::vector<std::shared_ptr<Process>> finishedProcesses;
    std::unordered_map<uint32_t, bool> cpuStatus;
    std::vector<std::thread> cpuThreads;
    std::thread schedulerThread;

    bool schedulerRunning = false;
    uint32_t cpuCycles = 0;
	std::condition_variable schedulerCv; // added for thread synchronization
	std::mutex schedulerCvMutex; //added for thread synchronization
    std::mutex queueMutex;
    std::mutex cpuMutex;
    std::mutex schedulerMutex;
};
