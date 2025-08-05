#pragma once
#include <cstdint>
#include <optional> 
#include <unordered_map>
#include <atomic>
#include <queue>
#include "Process.h"
#include <thread>
#include <mutex>
#include <random>

class CPUScheduler
{
public:
	CPUScheduler();
	void runScheduler();
	void startCPU(std::shared_ptr<Process> process, int cpuID);
	uint32_t getCpuCycles();
	uint32_t getActiveCPUTicks();
	uint32_t getIdleCPUTicks();
	void cpuScheduling();
	void batchProcessing();
	void cpuWorker(std::shared_ptr<Process> process, int cpuID, std::string algo);
	void addProcessToReadyQueue(std::shared_ptr<Process> process);
	void startSchedulingProcesses();
	void stopSchedulingProcesses();
	void screenList();
	void reportUtil();
	std::shared_ptr<Process> getNextProcess();
	void addToRunningProcesses(std::shared_ptr<Process> runningProcess);
	void addProcessesToFinishedProcessesFromRunningProcessesList();
	void printReadyQueueForProcesses();
	std::vector<std::shared_ptr<Process>> getFinishedProcesses();
	std::vector<std::shared_ptr<Process>> getRunningProcesses();
	void printReadyQueue();
	void printFreeCPUs();
	std::optional<int> getFreeCPU();

	std::uint32_t getAmountOfWorkingCPUs();
	std::uint32_t getAmountOfCPUs();
	void incrementActiveIdleCPUTicks();

private:
	std::mutex readyQueueMutex;
	std::mutex cpuAssignmentMutex;
	std::mutex runningProcessesMutex;
	std::mutex finishedProcessesMutex;
	std::mutex cpuMutex;
	std::atomic<uint32_t> cpuCycles{ 0 };
	std::atomic<uint32_t> activeCPUTicks{ 0 };
	std::atomic<uint32_t> idleCPUTicks{ 0 };
	std::unordered_map<int, bool> isFreeCPUs;
	bool isBatchProcessing = false;
	std::queue<std::shared_ptr<Process>> readyQueueForProcesses;
	std::vector<std::shared_ptr<Process>> finishedProcesses;
	std::vector<std::shared_ptr<Process>> runningProcesses;
};

