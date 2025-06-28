#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include "ConfigSpecs.h"
#include <thread>
#include <unordered_map>

#include "ProcessManager.h"

class CPUScheduler {
public:
    explicit  CPUScheduler(std::shared_ptr<ConfigSpecs> config);

    virtual void schedule();
    void start();
    void stop();
    void addProcess(std::shared_ptr<Process> process);
    void schedulerThreadFunction();
    void runFCFS(uint32_t cpuId, std::shared_ptr<Process> proc);
    void runRR(uint32_t cpuId, std::shared_ptr<Process> proc);
    void moveToFinished(std::shared_ptr<Process> proc, uint32_t cpuId);
    int getUsedCPUCount();
    void getReportData(std::string& cpuStatsOut, std::vector<std::shared_ptr<Process>>& runningOut,
                       std::vector<std::shared_ptr<Process>>& finishedOut);
    void printReport(std::ostream& out);
    void printUtil();


private:

    std::thread schedulerThread;
    bool schedulerRunning = false;
    std::mutex schedulerMutex; // NEW: to protect schedulerRunning

    std::atomic<uint32_t> cpuCycles{ 0 };

    std::vector<std::thread> cpuThreads;
    std::unordered_map<uint32_t, bool> cpuStatus; // true = free, false = busy
    std::queue<std::shared_ptr<Process>> readyQueue;
    std::vector<std::shared_ptr<Process>> runningProcesses;
    std::vector<std::shared_ptr<Process>> finishedProcesses;

    std::mutex queueMutex;
    std::mutex cpuMutex;
    bool freeCPU = true ;
   std::shared_ptr<ConfigSpecs> config;
};
