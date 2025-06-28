#include "Process.h"

#include <fstream>
#include <iomanip>

Process::Process(int pID, std::string pName, std::vector<std::shared_ptr<Command>> cList)
{
    processID = pID;
    processName = std::move(pName);
    commandList = std::move(cList);
    processCreatedOn = std::time(nullptr);
    linesOfCode = static_cast<std::uint32_t>(commandList.size());
}

void Process::executeCurrentCommand(int cpuId) {
    if (currentInstructionLine < commandList.size()) {
        commandList[currentInstructionLine]->executeCommand(*this, cpuId);  // NEW version using stateful exec
    }
}

void Process::moveToNextLine() {
    if (currentInstructionLine < commandList.size()) {
        currentInstructionLine++;
    }
}

bool Process::isFinished()
{
	return currentInstructionLine == commandList.size();
}

void Process::assignCurrentCPUID(int cpuID)
{
    currentCPUID = cpuID;
}

void Process::setProcessState(ProcessState pState)
{
    processState = pState;
}

void Process::setProcessFinishedTime()
{
    processFinishedOn = std::time(nullptr);
}

int Process::getProcessID()
{
    return processID;
}

std::string Process::getProcessName()
{
    return processName;
}

std::uint32_t Process::getCurrentInstructionLine()
{
    return currentInstructionLine;
}

std::uint32_t Process::getLinesOfCode()
{
    return linesOfCode;
}

std::time_t Process::getProcessCreatedOn()
{
    return processCreatedOn;
}

std::time_t Process::getProcessFinishedOn()
{
    return processFinishedOn;
}

Process::ProcessState Process::getProcessState() {
    return processState;
}

int Process::getCurrentCPUID()
{
    return currentCPUID;
}
std::unordered_map<std::string, uint16_t>& Process::getVariables() {
    return variables;
}


