#include "BackingStore.h"
#include "Process.h" 
#include <fstream>
 #include <sstream>
#include <iostream>
#include <iomanip>

BackingStore::BackingStore(const std::string& backingStoreTextFile, const std::string& backingStoreLogFile)
{
    textFileName = backingStoreTextFile;
    logFileName = backingStoreLogFile;

    std::ofstream clearText(textFileName, std::ios::trunc);
    clearText.close();

    std::ofstream clearLog(logFileName, std::ios::trunc);
    clearLog.close();
}

void BackingStore::storeProcess(std::shared_ptr<Process> process)
{
    std::ostringstream oss;
    oss << process->getProcessID() << " "
        << process->getProcessName() << " "
        << process->getCurrentInstructionLine() << " "
        << process->getLinesOfCode() << " "
        << process->getProcessCreatedOn() << " "
        << process->getMemorySize() << " "
        << process->getFrameSize() << " "
        << process->getTotalPages();

    writeToFile(oss.str());
    logOperation("STORE " + process->getProcessName() + ":", process);
}

void BackingStore::loadProcess(std::shared_ptr<Process> process)
{
    logOperation("LOAD " + process->getProcessName() + ":", process);

    std::ifstream inFile(textFileName);
    std::ostringstream newContent;

    std::string line;
    std::string targetPID = std::to_string(process->getProcessID());

    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string pidStr;
        iss >> pidStr;

        if (pidStr != targetPID) {
            newContent << line << "\n";  // Keep only unmatched lines
        }
    }

    inFile.close();

    std::ofstream outFile(textFileName, std::ios::trunc); // Clear and overwrite original file
    outFile << newContent.str();
    outFile.close();
}


void BackingStore::logOperation(const std::string& operation, const std::shared_ptr<Process>& process)
{
    std::ostringstream oss;
    oss << operation << " "
        << process->getProcessID() << " "
        << process->getCurrentInstructionLine() << " "
        << process->getLinesOfCode() << " "
        << process->getProcessCreatedOn() << " "
        << process->getMemorySize() << " "
        << process->getFrameSize() << " "
        << process->getTotalPages();

    appendToLog(oss.str());
}

void BackingStore::writeToFile(const std::string& line)
{
    std::ofstream outFile(textFileName, std::ios::app);
    if (outFile.is_open()) {
        outFile << line << '\n';
        outFile.close();
    }
    else {
        return;
        //std::cerr << "[BackingStore] Failed to write to file: " << textFileName << '\n';
    }
}

void BackingStore::appendToLog(const std::string& logLine)
{
    std::ofstream logFileOut(logFileName, std::ios::app);
    if (logFileOut.is_open()) {
        logFileOut << logLine << '\n';
        logFileOut.close();
    }
    else {
        return;
        /*std::cerr << "[BackingStore] Failed to append to log file: " << logFileName << '\n';*/
    }
}
