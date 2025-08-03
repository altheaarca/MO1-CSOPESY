// CPUScheduler.cpp
#include "CPUScheduler.h"
#include "MemoryManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <optional>

CPUScheduler::CPUScheduler(std::shared_ptr<ConfigSpecs> config, std::shared_ptr<MemoryManager> memManager)
    : config(config), memoryManager(memManager) {
    uint32_t cpuCount = config->getNumCPU();
    for (uint32_t i = 0; i < cpuCount; ++i) {
        cpuStatus[i] = true;
    }
}

void CPUScheduler::start() {
    std::lock_guard<std::mutex> lock(schedulerMutex);
    if (schedulerRunning) {
        std::cout << "[Scheduler] Already running.\n\n";
        return;
    }

    schedulerRunning = true;
    schedulerThread = std::thread(&CPUScheduler::schedulerThreadFunction, this);
    std::cout << "[Scheduler] Started.\n\n";
}

void CPUScheduler::stop() {
    {
        std::lock_guard<std::mutex> lock(schedulerMutex);
        if (!schedulerRunning) {
            std::cout << "[Scheduler] Not running.\n\n";
            return;
        }
        schedulerRunning = false;
    }

    schedulerCv.notify_all(); // wake sleeping scheduler

    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }


    std::cout << "[Scheduler] Stopped.\n\n";
}

void CPUScheduler::schedule() {
    start();
}

void CPUScheduler::addProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(process);
}

void CPUScheduler::schedulerThreadFunction() {
    while (schedulerRunning || !readyQueue.empty() || !runningProcesses.empty()) {
        cpuCycles++;
        memoryManager->tickCycle();


        std::optional<uint32_t> freeCPU;
        {
            std::lock_guard<std::mutex> lock(cpuMutex);
            for (auto& [cpuId, isFree] : cpuStatus) {
                if (isFree) {
                    freeCPU = cpuId;
                    break;
                }
            }
        }

        if (freeCPU.has_value()) {
            std::shared_ptr<Process> proc = nullptr;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (!readyQueue.empty()) {
                    proc = readyQueue.front();
                    readyQueue.pop();
                }
            }

            if (proc) {
                uint32_t cpuId = freeCPU.value();
                {
                    std::lock_guard<std::mutex> lock(cpuMutex);
                    cpuStatus[cpuId] = false;
                }

                proc->assignCurrentCPUID(cpuId);
                proc->setProcessState(Process::RUNNING);
                runningProcesses.push_back(proc);

                std::string algo = config->getSchedulerType();
                if (algo == "rr") {
                    cpuThreads.emplace_back(&CPUScheduler::runRR, this, cpuId, proc);
                }
                else {
                    cpuThreads.emplace_back(&CPUScheduler::runFCFS, this, cpuId, proc);
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (!schedulerRunning) break;
        if (cpuCycles % config->getQuantumCycles() == 0) {
            memoryManager->generateMemorySnapshot(cpuCycles);
        }
    }
}

void CPUScheduler::runFCFS(uint32_t cpuId, std::shared_ptr<Process> proc) {
    uint32_t delay = config->getDelayPerExecution();
    if (!memoryManager->allocateMemory(proc->getProcessName())) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        addProcess(proc);
        return;
    }
    while (!proc->isFinished()) {
        proc->executeCurrentCommand(cpuId);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        proc->moveToNextLine();
    }

    proc->setProcessFinishedTime();
    proc->setProcessState(Process::FINISHED);
    moveToFinished(proc, cpuId);
}

void CPUScheduler::runRR(uint32_t cpuId, std::shared_ptr<Process> proc) {
    uint32_t delay = config->getDelayPerExecution();
    uint32_t quantum = config->getQuantumCycles();
    uint32_t executed = 0;

    if (!memoryManager->allocateMemory(proc->getProcessName())) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        addProcess(proc);
        return;
    }

    while (!proc->isFinished() && executed < quantum) {
        proc->executeCurrentCommand(cpuId);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        proc->moveToNextLine();
        executed++;
    }

    if (proc->isFinished()) {
        proc->setProcessFinishedTime();
        proc->setProcessState(Process::FINISHED);
    }
    else {
        proc->setProcessState(Process::PRE_EMPTED);
        addProcess(proc);
    }

    moveToFinished(proc, cpuId);
    memoryManager->generateMemorySnapshot(cpuCycles);
}

void CPUScheduler::moveToFinished(std::shared_ptr<Process> proc, uint32_t cpuId) {
    std::lock_guard<std::mutex> lock(cpuMutex);

    auto it = std::find(runningProcesses.begin(), runningProcesses.end(), proc);
    if (it != runningProcesses.end()) {
        runningProcesses.erase(it);
    }

    auto finishedIt = std::find(finishedProcesses.begin(), finishedProcesses.end(), proc);
    if (finishedIt == finishedProcesses.end()) {
        finishedProcesses.push_back(proc);
    }

    cpuStatus[cpuId] = true;
    memoryManager->deallocateMemory(proc->getProcessName());
}

void CPUScheduler::printReport(std::ostream& out) {
    std::uint32_t totalCores = config->getNumCPU();
    std::uint32_t usedCores = 0;

    {
        std::lock_guard<std::mutex> lock(cpuMutex);
        for (const auto& [cpuId, isFree] : cpuStatus) {
            if (!isFree) ++usedCores;
        }
    }

    float utilization = (totalCores == 0) ? 0.0f : static_cast<float>(usedCores) / totalCores * 100.0f;

    out << "CPU utilization: " << std::fixed << std::setprecision(2) << utilization << "%\n";
    out << "Cores used: " << usedCores << "\n";
    out << "Cores available: " << (totalCores - usedCores) << "\n";
    out << "-----------------------------------------------------\n";

    out << "Running processes:\n";
    for (const auto& proc : runningProcesses) {
        if (!proc) continue;
        std::time_t created = proc->getProcessCreatedOn();
        std::tm timeinfo{};

        localtime_s(&timeinfo, &created);

        out << std::left << std::setw(12) << proc->getProcessName()
            << "(" << std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p") << ")   "
            << "Core: " << proc->getCurrentCPUID() << "   "
            << proc->getCurrentInstructionLine() << " / " << proc->getLinesOfCode()
            << "\n";
    }

    out << "\nFinished processes:\n";
    for (const auto& proc : finishedProcesses) {
        if (!proc) continue;

        std::time_t finished = proc->getProcessFinishedOn();
        std::tm timeinfo{};
        localtime_s(&timeinfo, &finished);

        out << std::left << std::setw(12) << proc->getProcessName()
            << "(" << std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p") << ")   "
            << "Finished   "
            << proc->getLinesOfCode() << " / " << proc->getLinesOfCode()
            << "\n";
    }

    out << "-----------------------------------------------------\n";
}

void CPUScheduler::printUtil() {
    std::ofstream outFile("csopesy-log.txt");
    if (outFile.is_open()) {
        printReport(outFile);
        outFile.close();
        std::cout << "[Report] CPU Utilization report written to csopesy-report.txt\n\n";
    }
    else {
        std::cerr << "Failed to write to report file.\n";
    }
}

void CPUScheduler::printVMStat() {
    //TODO: display the ff: Total memory - Total main memory in bytes,Used memory - Total active memory used by processes, Free memory - Total free memory that can still be used by other processes.
	// Idle cpu ticks - Number of ticks wherein CPU cores remained idle, Active cpu ticks - Number of ticks wherein CPU cores are actually executing instructions, Total cpu ticks - Number of ticks that passed for all CPU cores.
	// Num paged in - Accumulated number of pages paged in, Num paged out - Accumulated number of pages paged out.

    std::lock_guard<std::mutex> lk1(cpuMutex);
    std::lock_guard<std::mutex> lk2(queueMutex);
    std::cout << "Pages Paged In:     " << memoryManager->getPagesPagedIn() << "\n";
    std::cout << "Pages Paged Out:    " << memoryManager->getPagesPagedOut() << "\n";


    std::cout << "\n=========================\n";
    std::cout << "         VMSTAT          \n";
    std::cout << "=========================\n";

    std::cout << "Running Processes:  " << runningProcesses.size() << "\n";
    std::cout << "Ready Queue Size:   " << readyQueue.size() << "\n";
    std::cout << "Finished Processes: " << finishedProcesses.size() << "\n";

    auto memMgr = memoryManager;
    uint32_t totalFree = 0, totalUsed = 0;

	for (const auto& block : memMgr->getMemoryBlocks()) {
        if (block.allocated)
            totalUsed += block.size;
        else
            totalFree += block.size;
    }

    std::cout << "Memory Used:        " << totalUsed / 1024 << " KB\n";
    std::cout << "Memory Free:        " << totalFree / 1024 << " KB\n";
    std::cout << "External Fragmentation: " << memMgr->getExternalFragmentation() / 1024 << " KB\n";
    std::cout << "=========================\n\n";
}
