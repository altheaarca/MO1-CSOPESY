#include "BackingStore.h"
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

BackingStore::BackingStore(const std::string& filename)
    : storeFilename(filename)
{
}

bool BackingStore::writePage(const std::string& processName,
    int pageNumber,
    const std::string& content)
{
    std::ofstream out(storeFilename, std::ios::app);
    if (!out.is_open()) return false;

    // Timestamp
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; localtime_s(&tm, &in_time_t);

    out << "==== Process: " << processName
        << " Page: " << pageNumber << " ====\n";
    out << "Timestamp: " << std::put_time(&tm, "%m/%d/%Y %I:%M:%S %p") << "\n";
    out << content << "\n\n";
    return true;
}

std::vector<std::string> BackingStore::readPage(const std::string& processName,
    int pageNumber)
{
    std::ifstream in(storeFilename);
    std::vector<std::string> section;
    if (!in.is_open()) return section;

    std::string line;
    std::string header = "==== Process: " + processName + " Page: " + std::to_string(pageNumber) + " ====";
    bool inSection = false;
    while (std::getline(in, line)) {
        if (!inSection) {
            if (line == header) {
                inSection = true;
            }
        }
        else {
            if (line.rfind("====", 0) == 0) break; // next section
            section.push_back(line);
        }
    }
    return section;
}

void BackingStore::logEviction(const std::string& processName,
    int pageNumber)
{
    std::ofstream out(storeFilename, std::ios::app);
    if (!out.is_open()) return;

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{}; localtime_s(&tm, &in_time_t);

    out << "[Eviction] Process " << processName
        << " page " << pageNumber
        << " at " << std::put_time(&tm, "%m/%d/%Y %I:%M:%S %p")
        << "\n\n";
}
