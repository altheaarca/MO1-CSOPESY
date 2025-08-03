// MemoryManager.h
#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <memory>
#include "ConfigSpecs.h"
#include "BackingStore.h"

class MemoryManager {
public:
    struct MemoryBlock {
        uint32_t startAddress;
        uint32_t size;
        bool allocated;
        std::string processName;
    };

    MemoryManager(std::shared_ptr<ConfigSpecs> config);
    bool allocateMemory(const std::string& processName);
    void deallocateMemory(const std::string& processName);
    void mergeFreeBlocks();
    uint32_t getExternalFragmentation() const;
    void generateMemorySnapshot(uint32_t quantumCycle);
    void printMemoryStatus();
    const std::vector<MemoryBlock>& getMemoryBlocks() const;

    // Demand‐paging API
    void tickCycle();
    bool loadPage(const std::string& processName, int pageNumber, bool isModified);
    bool replacePage(const std::string& processName, int pageNumber, bool isModified);

    // Introspection
    uint32_t getPagesPagedIn()  const { return pagesPagedIn; }
    uint32_t getPagesPagedOut() const { return pagesPagedOut; }
    uint32_t getFrameSize()     const { return frameSize; }
    uint32_t getCpuCycles()     const { return cpuCycles; }

private:
    struct Frame {
        bool occupied = false;
        std::string ownerProcess;
        int pageNumber = -1;
        bool modified = false;
        uint32_t lastUsedCycle = 0;
    };

    std::shared_ptr<BackingStore> backingStore;
    std::vector<MemoryBlock>      memoryBlocks;
    std::vector<Frame>            frameTable;
    std::queue<int>               fifoQueue;

    uint32_t totalMemory = 0;
    uint32_t memPerProc = 0;
    uint32_t frameSize = 0;
    uint32_t cpuCycles = 0;
    uint32_t pagesPagedIn = 0;
    uint32_t pagesPagedOut = 0;

    std::mutex mutex_;
};
