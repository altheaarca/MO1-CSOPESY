#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <unordered_map>

class Process;

class BackingStore {
public:
    BackingStore(const std::string& backingStoreTextFile, const std::string& backingStoreLogFile);

    void storeProcess(std::shared_ptr<Process> process);
    void loadProcess(std::shared_ptr<Process> process);
    void logOperation(const std::string& operation, const std::shared_ptr<Process>& process);

private:
    std::string textFileName;
    std::string logFileName;

    void writeToFile(const std::string& line);
    void appendToLog(const std::string& logLine);
};
