#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
#include <optional>
#include "Command.h"
#include <ctime>

class Process
{
public:
    Process(int pID, std::string pName, std::vector<std::shared_ptr<Command>> cList);
    Process(int pID, std::string pName, std::uint32_t processSize, std::vector<std::shared_ptr<Command>> cList);
    Process(int pID, std::string pName, std::string commands);
    Process(int pID, std::string pName, std::uint32_t processSize, std::string commands);

    enum ProcessState {
        NEW,
        RUNNING,
        PRE_EMPTED,
        FINISHED,
        STOPPED
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


    std::uint32_t randomProcessSize(std::uint32_t minProcessSize, std::uint32_t maxProcessSize);
    std::uint32_t getMemorySize();
    std::uint32_t getFrameSize();
    std::uint32_t getTotalPages();

    std::vector<std::vector<std::string>> parseCompoundCommand(const std::string& compound);
    std::vector<std::string> tokenizeForCompound(const std::string& command);
    std::string cleanQuotedString(const std::string& str);

    void initializeVirtualMemoryMap();

    void insertValueToMemory(std::string memoryAddress, std::int16_t value);
    void insertValueFromVarToMemory(std::string memoryAddress, std::string var);
    void insertValueFromAddressToVar(std::string var, std::string memoryAddress);
    void insertValueToVar(std::string var, std::int16_t value);
    std::optional<std::int16_t> getValueFromAddress(std::string memoryAddress);
    std::optional<std::int16_t> getValueFromVar(std::string var);

    void createCommandsFromStringOfCommands();

    bool isValue(std::string token);
    bool isMemoryAddressFormat(std::string token);
    bool isVariable(std::string token);

    void viewSymbolTableAndMemorySpace();
    void logPrintStatements(std::string print);
    void viewLogPrintStatements();

    bool isProcessStoppedDueToMemoryAccessError();
    void stopProcess(std::string memoryAddress);
    std::string getInvalidMemoryAccess();
    std::time_t getProcessStoppedOn();

    std::uint32_t increasingValue = 0;
private:
    ProcessState processState = NEW;
    std::string processName;
    int processID = 0;
    std::time_t processCreatedOn = 0;
    std::time_t processFinishedOn = 0;
    std::uint32_t currentInstructionLine = 0;
    std::uint32_t linesOfCode = 0;
    std::vector<std::shared_ptr<Command>> commandList;
    std::vector<std::string> printLogs;
    int currentCPUID = -1;

    std::time_t processStoppedOn = 0;
    std::string invalidMemoryAccessError = "";
    bool processStoppedDueToMemoryAccessError = false;
    std::uint32_t processSize;
    std::uint32_t frameSize;
    std::uint32_t totalPages;
    std::vector<std::vector<std::string>> stringCommands;
    std::unordered_map<std::string, std::int16_t> symbolTable;
    std::unordered_map<std::string, std::int16_t> virtualMemorySpace;
};