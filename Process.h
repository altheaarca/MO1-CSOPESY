#pragma once

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include "Command.h"

class Process
{
public:
    Process(int pID, std::string pName, std::vector<std::shared_ptr<Command>> cList);

    enum ProcessState {
        NEW,
        RUNNING,
        PRE_EMPTED,
        FINISHED
    };

    void executeCurrentCommand();
    void moveToNextLine();
    bool isFinished();
    void assignCurrentCPUID(int cpuID);
    void setProcessState(ProcessState pState);
    void setProcessFinishedTime();

    int getProcessID();
    std::string getProcessName();
    std::uint32_t getCurrentInstructionLine();
    std::uint32_t getLinesOfCode();
    std::time_t getProcessCreatedOn();
    std::time_t getProcessFinishedOn();
    ProcessState getProcessState();
    int getCurrentCPUID();
    const std::vector<std::string>& getPrintLogs();

private:
    ProcessState processState = NEW;
    std::string processName;
    int processID = 0;
    std::uint32_t currentInstructionLine = 0;
    std::uint32_t linesOfCode = 0;
    std::time_t processCreatedOn = 0;
    std::time_t processFinishedOn = 0;
    std::vector<std::shared_ptr<Command>> commandList;
    std::vector<std::string> printLogs;
    int currentCPUID = -1;
};

