#pragma once
#include <cstdint>      
#include <memory>       
#include <unordered_map> 
#include <vector>      

class Process;

class MemoryManager
{
public:
	MemoryManager(std::uint32_t maxMem, std::uint32_t memFrame);
	bool isAllocatable(std::uint32_t processPages);
	void allocateProcess(std::shared_ptr<Process> process);
	void deallocateProcess(std::shared_ptr<Process> process);
	bool isProcessAllocated(std::shared_ptr<Process> process);
	void viewMemoryPages();
	std::shared_ptr<Process> getOldestProcess();
	std::uint32_t getTotalMemory();
	std::uint32_t getFrameSize();
	std::uint32_t getTotalPages();
	std::uint32_t getFramesPagedIn();
	std::uint32_t getFramesPagedOut();
	std::uint32_t getTotalUsedMemory();

	void virtualMemoryStat();
	void processInformation();
private:
	std::uint32_t totalMemory;
	std::uint32_t totalUsedMemory;
	std::uint32_t frameSize;
	std::uint32_t totalPages;
	std::uint32_t framesPagedIn;
	std::uint32_t framesPagedOut;
	std::unordered_map<int, int> pageTable;
	std::vector<std::shared_ptr<Process>> processesAllocatedInMemory;
};

