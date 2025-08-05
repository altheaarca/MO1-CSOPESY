#include "MemoryManager.h"
#include "Process.h"
#include "OSController.h"
#include <iostream>
#include <iomanip>
#define NOMINMAX


MemoryManager::MemoryManager(std::uint32_t maxMem, std::uint32_t memFrame)
{
	totalMemory = maxMem;
	frameSize = memFrame;
	totalPages = totalMemory / frameSize;
	framesPagedIn = 0;
	framesPagedOut = 0;
	totalUsedMemory = 0;

	// Initialize all pages to -1 (free)
	for (std::uint32_t i = 0; i < totalPages; ++i) {
		pageTable[i] = -1;
	}
}

bool MemoryManager::isAllocatable(std::uint32_t processPages)
{
	std::uint32_t freePages = 0;
	for (const auto& [page, pid] : pageTable) {
		if (pid == -1) ++freePages;
	}
	return freePages >= processPages;
}

void MemoryManager::allocateProcess(std::shared_ptr<Process> process)
{
	int pid = process->getProcessID();
	std::uint32_t processSize = process->getMemorySize();
	std::uint32_t pagesNeeded = process->getTotalPages();

	// Check if already allocated
	for (const auto& p : processesAllocatedInMemory) {
		if (p->getProcessID() == pid) {
			//std::cout << "[MemoryManager] PID " << pid << " is already allocated in memory.\n";
			return;
		}
	}

	// Check if memory is sufficient
	if (!isAllocatable(pagesNeeded)) {
		//std::cout << "[MemoryManager] Not enough memory to allocate PID " << pid << "\n";
		return;
	}

	std::uint32_t pagesAllocated = 0;
	for (auto& [pageNum, mappedPID] : pageTable) {
		if (mappedPID == -1) {
			pageTable[pageNum] = pid;
			pagesAllocated++;
			framesPagedIn++;

			if (pagesAllocated == pagesNeeded)
				break;
		}
	}

	totalUsedMemory += pagesAllocated * frameSize;
	processesAllocatedInMemory.push_back(process);
	//std::cout << "[MemoryManager] Allocated PID " << pid << " with " << pagesAllocated << " page(s).\n";
}

void MemoryManager::deallocateProcess(std::shared_ptr<Process> process)
{
	int pid = process->getProcessID();
	std::uint32_t freedPages = 0;

	for (auto& [pageNum, mappedPID] : pageTable) {
		if (mappedPID == pid) {
			pageTable[pageNum] = -1;
			freedPages++;
			framesPagedOut++;
		}
	}

	// Remove from allocation list
	processesAllocatedInMemory.erase(
		std::remove_if(processesAllocatedInMemory.begin(), processesAllocatedInMemory.end(),
			[pid](const std::shared_ptr<Process>& p) {
				return p->getProcessID() == pid;
			}),
		processesAllocatedInMemory.end()
	);
	totalUsedMemory -= freedPages * frameSize;
	//std::cout << "[MemoryManager] Deallocated PID " << pid << " and freed " << freedPages << " page(s).\n";
}

bool MemoryManager::isProcessAllocated(std::shared_ptr<Process> process)
{
	int pid = process->getProcessID();
	for (const auto& [pageNum, mappedPID] : pageTable) {
		if (mappedPID == pid) {
			/*	std::cout << "[MemoryManager] Process PID " << pid << " is currently allocated in memory.\n";*/
			return true;
		}
	}
	//std::cout << "[MemoryManager] Process PID " << pid << " is NOT allocated in memory.\n";
	return false;
}

void MemoryManager::viewMemoryPages()
{
	std::cout << "\n--- Memory Page Table ---\n";
	std::unordered_map<int, std::uint32_t> memoryUsagePerPID;
	std::uint32_t usedPages = 0;

	for (const auto& [pageNum, pid] : pageTable) {
		if (pid == -1) {
			std::cout << "Page " << pageNum << " => [ FREE ]\n";
		}
		else {
			std::cout << "Page " << pageNum << " => PID " << pid << "\n";
			memoryUsagePerPID[pid]++;
			usedPages++;
		}
	}

	std::cout << "--------------------------\n";

	std::cout << "\n--- Memory Usage Per Process ---\n";
	for (const auto& [pid, pages] : memoryUsagePerPID) {
		std::uint32_t bytesUsed = pages * frameSize;
		std::cout << "PID " << pid << " => " << pages << " pages (" << bytesUsed << " bytes)\n";
	}

	std::uint32_t totalPagesBytes = totalPages * frameSize;
	std::uint32_t usedBytes = usedPages * frameSize;
	double usagePercent = (static_cast<double>(usedPages) / totalPages) * 100.0;

	std::cout << "\n--- Overall Memory Usage ---\n";
	std::cout << "Used: " << usedPages << "/" << totalPages << " pages\n";
	std::cout << "Used: " << usedBytes << "/" << totalPagesBytes << " bytes\n";
	std::cout << "Usage: " << std::fixed << std::setprecision(2) << usagePercent << "%\n";
	std::cout << "Paged In: " << framesPagedIn << " pages\n";
	std::cout << "Paged Out: " << framesPagedOut << " pages\n";
	std::cout << "------------------------------\n";

}

std::shared_ptr<Process> MemoryManager::getOldestProcess()
{
	std::shared_ptr<Process> oldest = nullptr;
	std::time_t oldestTime = (std::numeric_limits<std::time_t>::max)();

	for (const auto& process : processesAllocatedInMemory) {
		if (process->getProcessState() == Process::PRE_EMPTED) {
			std::time_t created = process->getProcessCreatedOn();
			if (created < oldestTime) {
				oldestTime = created;
				oldest = process;
			}
		}
	}

	return oldest;
}

std::uint32_t MemoryManager::getTotalMemory()
{
	return totalMemory;
}

std::uint32_t MemoryManager::getFrameSize()
{
	return frameSize;
}

std::uint32_t MemoryManager::getTotalPages()
{
	return totalPages;
}

std::uint32_t MemoryManager::getFramesPagedIn()
{
	return framesPagedIn;
}

std::uint32_t MemoryManager::getFramesPagedOut()
{
	return framesPagedOut;
}

std::uint32_t MemoryManager::getTotalUsedMemory()
{
	return totalUsedMemory;
}

void MemoryManager::virtualMemoryStat()
{
	auto scheduler = OSController::getInstance()->getCPUScheduler();

	std::uint32_t cpuCycles = scheduler->getCpuCycles();
	std::uint32_t activeCPUTicks = scheduler->getActiveCPUTicks();
	std::uint32_t idleCPUTicks = scheduler->getIdleCPUTicks();

	std::uint32_t pagedIn = getFramesPagedIn();
	std::uint32_t pagedOut = getFramesPagedOut();

	std::uint32_t usedMemory = getTotalUsedMemory();
	std::uint32_t totalMemory = totalPages * frameSize;
	std::uint32_t freeMemory = totalMemory - usedMemory;

	std::stringstream ss;
	ss << "------------------------------------------\n";
	ss << "|              VMSTAT V01.00             |\n";
	ss << "------------------------------------------\n";
	ss << "Total Memory     : " << totalMemory << "B\n";
	ss << "Used Memory      : " << usedMemory << "B\n";
	ss << "Free Memory      : " << freeMemory << "B\n";
	ss << "Idle CPU Ticks   : " << idleCPUTicks << "\n";
	ss << "Active CPU Ticks : " << activeCPUTicks << "\n";
	ss << "Total CPU Ticks  : " << cpuCycles << "\n";
	ss << "Num Paged In     : " << pagedIn << "\n";
	ss << "Num Paged Out    : " << pagedOut << "\n";
	ss << "------------------------------------------\n\n";

	std::cout << ss.str();
}

void MemoryManager::processInformation()
{
	auto scheduler = OSController::getInstance()->getCPUScheduler();
	std::uint32_t workingCPUs = scheduler->getAmountOfWorkingCPUs();
	std::uint32_t amountOfCPUs = scheduler->getAmountOfCPUs();

	std::ostringstream ss;

	// CPU utilization
	double cpuUtilPercent = 0.0;
	if (amountOfCPUs != 0)
		cpuUtilPercent = (static_cast<double>(workingCPUs) / amountOfCPUs) * 100.0;

	// Memory usage
	std::uint32_t usedMemory = getTotalUsedMemory();
	std::uint32_t totalMemory = totalPages * frameSize;
	double memoryUtil = (totalMemory != 0) ? (static_cast<double>(usedMemory) / totalMemory) * 100.0 : 0.0;

	// Build memory usage map per PID
	std::unordered_map<int, std::uint32_t> memoryUsagePerPID;
	for (const auto& [pageNum, pid] : pageTable) {
		if (pid != -1) {
			memoryUsagePerPID[pid]++;
		}
	}

	// Output header
	ss << "-------------------------------------------\n";
	ss << "|            PROCESS-SMI V01.00           |\n";
	ss << "-------------------------------------------\n";
	ss << "CPU-Util      : " << std::fixed << std::setprecision(2) << cpuUtilPercent << "%\n";
	ss << "Memory Usage  : " << usedMemory << "B / " << totalMemory << "B\n";
	ss << "Memory Util   : " << std::fixed << std::setprecision(2) << memoryUtil << "%\n\n";
	ss << "===========================================\n";
	ss << "Running processes and memory usage:\n";
	ss << "-------------------------------------------\n";

	for (const auto& proc : processesAllocatedInMemory) {
		std::string procName = proc->getProcessName();
		std::uint32_t pid = proc->getProcessID();
		std::uint32_t memBytes = memoryUsagePerPID[pid] * frameSize;
		ss << procName << " (" << memBytes << "B)\n";
	}

	ss << "-------------------------------------------\n\n";

	std::cout << ss.str();
}


