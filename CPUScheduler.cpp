#include "CPUScheduler.h"
#include "OSController.h"
#include <fstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <chrono>
#include <queue>
#include <vector>
#include <optional>
#include <sstream>
#include <random>

CPUScheduler::CPUScheduler() {
}

void CPUScheduler::runScheduler() {
	auto config = OSController::getInstance()->getConfig();
	if (!config) {
		std::cerr << "[ERROR] Config is null — make sure to inject it before calling runScheduler().\n";
		return;
	}

	for (std::uint32_t i = 0; i < config->getNumCPU(); ++i) {
		isFreeCPUs[i] = true;
	}

	std::thread([this]() {
		this->cpuScheduling();
		}).detach();
}

void CPUScheduler::startCPU(std::shared_ptr<Process> process, int cpuID)
{
	auto config = OSController::getInstance()->getConfig();
	std::string schedulingAlgo = config->getSchedulerType();
	std::uint32_t delayPerExecution = config->getDelayPerExecution() + 1;

	if (schedulingAlgo == "fcfs" || schedulingAlgo == "rr") {
		process->setProcessState(Process::RUNNING);
		process->assignCurrentCPUID(cpuID);

		std::thread([this, process, cpuID, schedulingAlgo]() {
			cpuWorker(process, cpuID, schedulingAlgo);
			}).detach();
	}
}

uint32_t CPUScheduler::getCpuCycles()
{
	return cpuCycles.load();
}

uint32_t CPUScheduler::getActiveCPUTicks()
{
	return activeCPUTicks.load();
}

uint32_t CPUScheduler::getIdleCPUTicks()
{
	return idleCPUTicks.load();
}

void CPUScheduler::cpuScheduling()
{
	auto pagingAllocator = OSController::getInstance()->getMemoryManager();
	auto backingStore = OSController::getInstance()->getBackingStore();
	while (true) {
		cpuCycles++;
		incrementActiveIdleCPUTicks();
		std::this_thread::sleep_for(std::chrono::microseconds(100));

		addProcessesToFinishedProcessesFromRunningProcessesList();

		auto maybeFreeCPU = getFreeCPU();
		if (maybeFreeCPU.has_value() && !readyQueueForProcesses.empty()) {
			std::shared_ptr<Process> process = nullptr;

			{
				std::lock_guard<std::mutex> lock(readyQueueMutex);
				process = readyQueueForProcesses.front();
				readyQueueForProcesses.pop();
			}

			if (process) {
				if (!pagingAllocator->isProcessAllocated(process)) {

					backingStore->loadProcess(process);

					bool freeSpace = pagingAllocator->isAllocatable(process->getTotalPages());

					if (freeSpace) {
						pagingAllocator->allocateProcess(process);

						bool isPaused = (process->getProcessState() == Process::PRE_EMPTED);

						{
							std::lock_guard<std::mutex> lock(cpuAssignmentMutex);
							startCPU(process, *maybeFreeCPU);
						}

						if (process->getProcessState() == Process::RUNNING) {
							if (!isPaused) {
								addToRunningProcesses(process);
							}
						}
					}
					else {
						auto backingStoreProcess = pagingAllocator->getOldestProcess();

						if (!backingStoreProcess) {
							addProcessToReadyQueue(process);
							continue;
						}

						pagingAllocator->deallocateProcess(backingStoreProcess);
						backingStore->storeProcess(backingStoreProcess);

						bool freeSpace = pagingAllocator->isAllocatable(process->getTotalPages());

						if (freeSpace) {
							pagingAllocator->allocateProcess(process);
							bool isPaused = (process->getProcessState() == Process::PRE_EMPTED);

							{
								std::lock_guard<std::mutex> lock(cpuAssignmentMutex);
								startCPU(process, *maybeFreeCPU);
							}

							if (process->getProcessState() == Process::RUNNING) {
								if (!isPaused) {
									addToRunningProcesses(process);
								}
							}
						}
					}
				}
				else {
					bool isPaused = (process->getProcessState() == Process::PRE_EMPTED);

					{
						std::lock_guard<std::mutex> lock(cpuAssignmentMutex);
						startCPU(process, *maybeFreeCPU);
					}

					if (process->getProcessState() == Process::RUNNING) {
						if (!isPaused) {
							addToRunningProcesses(process);
						}
					}
				}
			}
		}
	}
}

void CPUScheduler::batchProcessing()
{
	auto config = OSController::getInstance()->getConfig();
	auto consoleManager = OSController::getInstance()->getConsoleManager();
	auto commandManager = OSController::getInstance()->getCommandManager();
	auto processManager = OSController::getInstance()->getProcessManager();

	std::atomic_uint32_t lastCycle = 0;

	while (isBatchProcessing) {

		if ((getCpuCycles() - lastCycle) >= config->getBatchProcessFreq()) {
			lastCycle = getCpuCycles();

			auto c = commandManager->generateCommands();
			int id = consoleManager->getGlobalProcessID();
			consoleManager->incrementGlobalProcessID();

			std::string processName = "p" + std::to_string(id);

			auto newProcess = processManager->createProcess(id, processName, c);

			auto processConsole = std::make_shared<ProcessConsole>(processName, newProcess);

			consoleManager->createProcessConsole(processName, processConsole);

			addProcessToReadyQueue(newProcess);
		}
	}
}

void CPUScheduler::cpuWorker(std::shared_ptr<Process> process, int cpuID, std::string algo)
{
	{
		std::lock_guard<std::mutex> lock(cpuMutex);
		isFreeCPUs[cpuID] = false;
		process->assignCurrentCPUID(cpuID);
	}

	auto pagingAllocator = OSController::getInstance()->getMemoryManager();
	auto config = OSController::getInstance()->getConfig();
	std::string schedulingAlgo = config->getSchedulerType();
	std::uint32_t delayPerExecution = config->getDelayPerExecution() + 1;
	std::uint32_t maxCommandsPerRR = config->getQuantumCycles();
	std::uint32_t currentCommandRR = 0;

	uint32_t nextCycle = getCpuCycles() + delayPerExecution;

	if (algo == "fcfs") {
		while (!process->isFinished() && !process->isProcessStoppedDueToMemoryAccessError()) {
			while (getCpuCycles() < nextCycle) {
			}

			process->executeCurrentCommand();
			process->moveToNextLine();

			nextCycle += delayPerExecution;
		}

		if (process->isProcessStoppedDueToMemoryAccessError()) {
			pagingAllocator->deallocateProcess(process);
			{
				std::lock_guard<std::mutex> lock(cpuMutex);
				isFreeCPUs[cpuID] = true;
			}
			return;
		}

		process->setProcessState(Process::FINISHED);
		process->setProcessFinishedTime();
	}
	else if (algo == "rr") {
		while (!process->isFinished() && currentCommandRR < maxCommandsPerRR && !process->isProcessStoppedDueToMemoryAccessError()) {
			while (getCpuCycles() < nextCycle) {
			}

			process->executeCurrentCommand();
			process->moveToNextLine();
			currentCommandRR++;
			nextCycle += delayPerExecution;
		}

		if (process->isProcessStoppedDueToMemoryAccessError()) {
			pagingAllocator->deallocateProcess(process);
			{
				std::lock_guard<std::mutex> lock(cpuMutex);
				isFreeCPUs[cpuID] = true;
			}
			return;
		}

		if (!process->isFinished()) {
			process->setProcessState(Process::PRE_EMPTED);
			process->assignCurrentCPUID(-1);
			addProcessToReadyQueue(process);
		}
		else {
			process->setProcessFinishedTime();
			process->setProcessState(Process::FINISHED);
		}
	}
	{
		std::lock_guard<std::mutex> lock(cpuMutex);
		isFreeCPUs[cpuID] = true;
	}
}

void CPUScheduler::addProcessToReadyQueue(std::shared_ptr<Process> process)
{
	{
		std::lock_guard<std::mutex> lock(readyQueueMutex);
		readyQueueForProcesses.push(process);
	}
}

void CPUScheduler::startSchedulingProcesses()
{
	if (isBatchProcessing) {
		std::cerr << "Error: Batch processing has already started.\n";
		return;
	}

	isBatchProcessing = true;

	std::thread([this]() {
		this->batchProcessing();
		}).detach();
}

void CPUScheduler::stopSchedulingProcesses()
{
	if (!isBatchProcessing) {
		std::cerr << "Error: Batch processing is not running.\n";
		return;
	}

	isBatchProcessing = false;
}

void CPUScheduler::screenList()
{
	std::ostringstream oss;

	// --- CPU UTILIZATION ---
	std::uint32_t coresUsed = 0;
	std::uint32_t coresTotal = static_cast<std::uint32_t>(isFreeCPUs.size());

	{
		std::lock_guard<std::mutex> lock(cpuMutex);
		for (const auto& [cpuId, isFree] : isFreeCPUs) {
			if (!isFree) ++coresUsed;
		}
	}

	std::uint32_t coresAvailable = coresTotal - coresUsed;
	float utilization = (coresTotal > 0) ? (100.0f * coresUsed / coresTotal) : 0.0f;

	oss << std::fixed << std::setprecision(2);
	oss << "CPU Utilization: " << utilization << "%\n";
	oss << "Cores used: " << coresUsed << "\n";
	oss << "Cores available: " << coresAvailable << "\n\n";

	// --- RUNNING PROCESSES ---
	auto running = getRunningProcesses();

	oss << "--------------------------\n";
	oss << "Running processes:\n";

	for (const auto& process : running) {
		if (process->getCurrentCPUID() == -1) continue;

		std::time_t created = process->getProcessCreatedOn();
		std::tm timeinfo{};
		localtime_s(&timeinfo, &created);

		oss << std::left << std::setw(12) << process->getProcessName()
			<< " (" << std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p") << ")   "
			<< "Core: " << process->getCurrentCPUID() << "   "
			<< process->getCurrentInstructionLine() << "/" << process->getLinesOfCode()
			<< "\n";
	}

	oss << "\n";


	// --- FINISHED PROCESSES ---
	auto finished = getFinishedProcesses();

	oss << "Finished processes:\n";
	for (const auto& process : finished) {
		std::time_t finishedTime = process->getProcessFinishedOn();
		std::tm timeinfo{};
		localtime_s(&timeinfo, &finishedTime);

		oss << std::left << std::setw(12) << process->getProcessName()
			<< " (" << std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p") << ")   "
			<< "Finished  " << process->getCurrentInstructionLine() << "/" << process->getLinesOfCode()
			<< "\n";
	}
	oss << std::endl;


	// Output all at once
	std::cout << oss.str();
}

void CPUScheduler::reportUtil()
{
	std::ofstream logFile("csopesy-log.txt"); // overwrite file
	if (!logFile.is_open()) {
		std::cerr << "[ERROR] Failed to open csopesy-log.txt for writing.\n";
		return;
	}

	// --- CPU UTILIZATION ---
	std::uint32_t coresUsed = 0;
	std::uint32_t coresTotal = static_cast<std::uint32_t>(isFreeCPUs.size());

	{
		std::lock_guard<std::mutex> lock(cpuMutex);
		for (const auto& [cpuId, isFree] : isFreeCPUs) {
			if (!isFree) ++coresUsed;
		}
	}

	std::uint32_t coresAvailable = coresTotal - coresUsed;
	float utilization = (coresTotal > 0) ? (100.0f * coresUsed / coresTotal) : 0.0f;

	logFile << std::fixed << std::setprecision(2);
	logFile << "CPU Utilization: " << utilization << "%\n";
	logFile << "Cores used: " << coresUsed << "\n";
	logFile << "Cores available: " << coresAvailable << "\n\n";

	// --- RUNNING PROCESSES ---
	auto running = getRunningProcesses();

	logFile << "--------------------------\n";
	logFile << "Running processes:\n";

	for (const auto& process : running) {
		if (process->getCurrentCPUID() == -1)
			continue;

		std::time_t created = process->getProcessCreatedOn();
		std::tm timeinfo{};
		localtime_s(&timeinfo, &created);

		logFile << std::left << std::setw(12) << process->getProcessName()
			<< " (" << std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p") << ")   "
			<< "Core: " << process->getCurrentCPUID() << "   "
			<< process->getCurrentInstructionLine() << "/" << process->getLinesOfCode()
			<< "\n";
	}

	logFile << "\n";


	// --- FINISHED PROCESSES ---
	auto finished = getFinishedProcesses();

	logFile << "Finished processes:\n";
	for (const auto& process : finished) {
		std::time_t finishedTime = process->getProcessFinishedOn();
		std::tm timeinfo{};
		localtime_s(&timeinfo, &finishedTime);

		logFile << std::left << std::setw(12) << process->getProcessName()
			<< " (" << std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p") << ")   "
			<< "Finished  " << process->getCurrentInstructionLine() << "/" << process->getLinesOfCode()
			<< "\n";
	}
	logFile << std::endl;


	logFile.close();
}

std::shared_ptr<Process> CPUScheduler::getNextProcess()
{
	std::shared_ptr<Process> nextProcess = nullptr;
	{
		std::lock_guard<std::mutex> lock(readyQueueMutex);
		nextProcess = readyQueueForProcesses.front();
		readyQueueForProcesses.pop();
	}

	return nextProcess;
}

void CPUScheduler::addToRunningProcesses(std::shared_ptr<Process> runningProcess)
{
	runningProcesses.push_back(runningProcess);
}

void CPUScheduler::addProcessesToFinishedProcessesFromRunningProcessesList()
{
	auto pagingAllocator = OSController::getInstance()->getMemoryManager();
	std::lock_guard<std::mutex> lock(runningProcessesMutex);
	auto it = runningProcesses.begin();

	while (it != runningProcesses.end()) {
		auto process = *it;
		if (process->getProcessState() == Process::FINISHED) {
			pagingAllocator->deallocateProcess(process); // Deallocate memory

			{
				std::lock_guard<std::mutex> lock2(finishedProcessesMutex);
				finishedProcesses.push_back(process);
			}
			it = runningProcesses.erase(it);
		}
		else {
			++it;
		}
	}
}


void CPUScheduler::printReadyQueueForProcesses()
{
	std::queue<std::shared_ptr<Process>> tempQueue = readyQueueForProcesses;

	while (!tempQueue.empty()) {
		std::cout << tempQueue.front()->getProcessName() << " ";
		tempQueue.pop();
	}

	std::cout << std::endl;
}

std::vector<std::shared_ptr<Process>> CPUScheduler::getFinishedProcesses()
{
	std::lock_guard<std::mutex> lock(finishedProcessesMutex);
	return finishedProcesses; // Return a **copy** of the actual vector
}


std::vector<std::shared_ptr<Process>> CPUScheduler::getRunningProcesses()
{
	std::lock_guard<std::mutex> lock(runningProcessesMutex);
	return runningProcesses; // Return a **copy** of the actual vector
}

void CPUScheduler::printReadyQueue()
{
	std::queue<std::shared_ptr<Process>> tempQueue = readyQueueForProcesses;

	while (!tempQueue.empty())
	{
		std::shared_ptr<Process> process = tempQueue.front();
		tempQueue.pop();

		if (process) {
			std::cout << process->getProcessName() << std::endl;
		}
	}
}

void CPUScheduler::printFreeCPUs()
{
	std::lock_guard<std::mutex> lock(cpuMutex); // 🔒 Ensure thread-safe access

	std::cout << "CPU Core Status:\n";
	std::cout << "----------------\n";

	for (const auto& [cpuId, isFree] : isFreeCPUs) {
		std::cout << "CPU " << std::setw(2) << cpuId << ": "
			<< (isFree ? "Free" : "In Use") << "\n";
	}

	std::cout << std::endl;
}

std::optional<int> CPUScheduler::getFreeCPU()
{
	std::lock_guard<std::mutex> lock(cpuMutex);

	std::vector<int> freeCPUs;
	for (const auto& [cpuId, isFree] : isFreeCPUs) {
		if (isFree) {
			freeCPUs.push_back(cpuId);
		}
	}

	if (freeCPUs.empty()) {
		return std::nullopt;
	}

	// Pick one at random
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, static_cast<int>(freeCPUs.size() - 1));

	return freeCPUs[dist(gen)];
}

std::uint32_t CPUScheduler::getAmountOfWorkingCPUs()
{
	std::lock_guard<std::mutex> lock(cpuMutex);
	std::uint32_t working = 0;

	for (const auto& [cpuId, isFree] : isFreeCPUs) {
		if (!isFree) ++working;
	}

	return working;
}

std::uint32_t CPUScheduler::getAmountOfCPUs()
{
	std::lock_guard<std::mutex> lock(cpuMutex);
	return static_cast<std::uint32_t>(isFreeCPUs.size());
}

void CPUScheduler::incrementActiveIdleCPUTicks()
{
	std::uint32_t workingCPUs = getAmountOfWorkingCPUs();

	if (workingCPUs == 0) {
		idleCPUTicks++;
	}
	else {
		activeCPUTicks++;
	}
}






