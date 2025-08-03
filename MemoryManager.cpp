// MemoryManager.cpp
#include "MemoryManager.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cmath>

MemoryManager::MemoryManager(std::shared_ptr<ConfigSpecs> config) {
    totalMemory = config->getMaxOverallMemory();
    memPerProc = config->getMemPerProcess();
    frameSize = config->getMemPerFrame();

    memoryBlocks.push_back({ 0, totalMemory, false, "" });

    // backing store + frame table
    backingStore = std::make_shared<BackingStore>("csopesy-backing-store.txt");
    uint32_t frameCount = totalMemory / frameSize;
    frameTable.resize(frameCount);
}

bool MemoryManager::allocateMemory(const std::string& processName) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (size_t i = 0; i < memoryBlocks.size(); ++i) {
        auto& b = memoryBlocks[i];
        if (!b.allocated && b.size >= memPerProc) {
            MemoryBlock nb{ b.startAddress, memPerProc, true, processName };
            b.startAddress += memPerProc;
            b.size -= memPerProc;
            if (b.size == 0) memoryBlocks[i] = nb;
            else memoryBlocks.insert(memoryBlocks.begin() + i, nb);
            return true;
        }
    }
    return false;
}

void MemoryManager::deallocateMemory(const std::string& processName) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& b : memoryBlocks) {
            if (b.allocated && b.processName == processName) {
                b.allocated = false;
                b.processName.clear();
            }
        }
        mergeFreeBlocks();
    }
    // free frames
    for (auto& f : frameTable) {
        if (f.occupied && f.ownerProcess == processName) {
            f = Frame{};
            pagesPagedOut++;
        }
    }
}

void MemoryManager::mergeFreeBlocks() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (size_t i = 0; i + 1 < memoryBlocks.size();) {
        if (!memoryBlocks[i].allocated && !memoryBlocks[i + 1].allocated) {
            memoryBlocks[i].size += memoryBlocks[i + 1].size;
            memoryBlocks.erase(memoryBlocks.begin() + i + 1);
        }
        else ++i;
    }
}

uint32_t MemoryManager::getExternalFragmentation() const {
    uint32_t totalFree = 0;
    for (auto& b : memoryBlocks) {
        if (!b.allocated && b.size < memPerProc)
            totalFree += b.size;
    }
    return totalFree;
}


void MemoryManager::generateMemorySnapshot(uint32_t quantumCycle) {
    std::ostringstream filename;
    filename << "memory_stamp_" << std::setw(2) << std::setfill('0') << quantumCycle << ".txt";
    std::ofstream out(filename.str());
    if (!out.is_open()) return;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo{};
    localtime_s(&timeinfo, &in_time_t);
    out << "Timestamp: (" << std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p") << ")\n";

    int procCount = 0;
    for (const auto& block : memoryBlocks) {
        if (block.allocated) ++procCount;
    }

    out << "Number of processes in memory: " << procCount << "\n";
    out << "Total external fragmentation in KB: " << getExternalFragmentation() / 1024 << "\n\n";

    out << "----end---- = " << totalMemory << "\n";
    for (auto it = memoryBlocks.rbegin(); it != memoryBlocks.rend(); ++it) {
        if (it->allocated) {
            out << (it->startAddress + it->size) << "\n";
            out << it->processName << "\n";
            out << it->startAddress << "\n\n";
        }
    }
    out << "----start---- = 0\n";
    out.close();
}

void MemoryManager::printMemoryStatus() {
    //TODO: use customizedLayout as reference for format 
    std::cout << "\n=========================\n";
    std::cout << "       PROCESS-SMI       \n";
    std::cout << "=========================\n";

    int procCount = 0;
    for (const auto& block : memoryBlocks) {
        if (block.allocated) {
            std::cout << "Process: " << std::setw(15) << block.processName
                << " | Start: " << std::setw(8) << block.startAddress
                << " | Size: " << std::setw(8) << block.size << "\n";
            ++procCount;
        }
    }

    std::cout << "\nTotal processes in memory: " << procCount << "\n";
    std::cout << "Total external fragmentation: " << getExternalFragmentation() / 1024 << " KB\n";
    std::cout << "=========================\n\n";
}

const std::vector<MemoryManager::MemoryBlock>& MemoryManager::getMemoryBlocks() const {
    return memoryBlocks;
}

void MemoryManager::tickCycle() {
    cpuCycles++;
}

bool MemoryManager::loadPage(const std::string& processName, int pageNumber, bool isModified) {
    // free frame?
    for (size_t i = 0; i < frameTable.size(); ++i) {
        if (!frameTable[i].occupied) {
            frameTable[i] = Frame{ true, processName, pageNumber, isModified, cpuCycles };
            fifoQueue.push((int)i);
            pagesPagedIn++;
            return true;
        }
    }
    // replace
    if (replacePage(processName, pageNumber, isModified)) {
        pagesPagedIn++;
        return true;
    }
    return false;
}

bool MemoryManager::replacePage(const std::string& processName, int pageNumber, bool isModified) {
    while (!fifoQueue.empty()) {
        int idx = fifoQueue.front(); fifoQueue.pop();
        auto& v = frameTable[idx];
        if (v.modified && (cpuCycles - v.lastUsedCycle) < frameTable.size()) {
            fifoQueue.push(idx);
            continue;
        }
        backingStore->logEviction(v.ownerProcess, v.pageNumber);
        pagesPagedOut++;
        backingStore->writePage(v.ownerProcess, v.pageNumber, "");
        v = Frame{ true, processName, pageNumber, isModified, cpuCycles };
        fifoQueue.push(idx);
        std::cout << "[Page Replacement] frame " << idx
            << " := " << processName << ":" << pageNumber << "\n";
        return true;
    }
    std::cerr << "[MemoryManager] no victim\n";
    return false;
}