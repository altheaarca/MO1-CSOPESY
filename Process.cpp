#include "Process.h"
#include "OSController.h"
#include <random>
#include <cmath> 
#include <chrono>
#include <iomanip>
#include <sstream>

Process::Process(int pID, std::string pName, std::vector<std::shared_ptr<Command>> cList)
{
    auto config = OSController::getInstance()->getConfig();
    std::uint32_t minProcessSize = config->getMinMemPerProc();
    std::uint32_t maxProcessSize = config->getMaxMemPerProc();
    processID = pID;
    processName = pName;
    commandList = std::move(cList);
    processCreatedOn = std::time(nullptr);
    linesOfCode = static_cast<std::uint32_t>(commandList.size());
    processSize = randomProcessSize(minProcessSize, maxProcessSize);
    frameSize = config->getMemPerFrame();
    totalPages = (processSize + frameSize - 1) / frameSize;
    // (64+128-1) / 128 -> 191 / 128 -> 1.49 == 1 (Para may page (1) pa din yung 64 kahit frame size is 128) (internal fragmentation)
    // (128+128-1) / 128 -> 255 / 128 -> 1.99 == 1 (truncate)
    // (256+128-1) / 128 -> 383 / 128 -> 2.99 == 2 (truncate)

    initializeVirtualMemoryMap();
}

Process::Process(int pID, std::string pName, std::uint32_t processSize, std::vector<std::shared_ptr<Command>> cList)
{
    auto config = OSController::getInstance()->getConfig();
    processID = pID;
    processName = pName;
    commandList = std::move(cList);
    processCreatedOn = std::time(nullptr);
    linesOfCode = static_cast<std::uint32_t>(commandList.size());
    this->processSize = processSize;
    frameSize = config->getMemPerFrame();
    totalPages = (processSize + frameSize - 1) / frameSize;
    initializeVirtualMemoryMap();
}

Process::Process(int pID, std::string pName, std::string commands)
{
    auto config = OSController::getInstance()->getConfig();
    std::uint32_t minProcessSize = config->getMinMemPerProc();
    std::uint32_t maxProcessSize = config->getMaxMemPerProc();
    processID = pID;
    processName = pName;
    std::string cleanedCommandsInput = cleanQuotedString(commands);
    auto parsedCommands = parseCompoundCommand(cleanedCommandsInput);
    stringCommands = parsedCommands;
    processCreatedOn = std::time(nullptr);
    linesOfCode = static_cast<std::uint32_t>(stringCommands.size());
    processSize = randomProcessSize(minProcessSize, maxProcessSize);
    frameSize = config->getMemPerFrame();
    totalPages = (processSize + frameSize - 1) / frameSize;
    initializeVirtualMemoryMap();
    createCommandsFromStringOfCommands();
}

Process::Process(int pID, std::string pName, std::uint32_t processSize, std::string commands)
{
    auto config = OSController::getInstance()->getConfig();
    processID = pID;
    processName = pName;
    std::string cleanedCommandsInput = cleanQuotedString(commands);
    auto parsedCommands = parseCompoundCommand(cleanedCommandsInput);
    stringCommands = parsedCommands;
    processCreatedOn = std::time(nullptr);
    linesOfCode = static_cast<std::uint32_t>(stringCommands.size());
    this->processSize = processSize;
    frameSize = config->getMemPerFrame();
    totalPages = (processSize + frameSize - 1) / frameSize;
    initializeVirtualMemoryMap();
    createCommandsFromStringOfCommands();
}

void Process::executeCurrentCommand() {
    if (currentInstructionLine < commandList.size()) {
        commandList[currentInstructionLine]->executeCommand2(*this);
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

const std::vector<std::string>& Process::getPrintLogs()
{
    return printLogs;
}

void Process::createCommandsFromStringOfCommands()
{
    for (std::size_t i = 0; i < stringCommands.size(); ++i) {
        const std::vector<std::string>& tokens = stringCommands[i];

        //std::cout << "Tokens: ";
        //for (const auto& token : tokens) {
        //    std::cout << "[" << token << "] ";
        //}
        //std::cout << std::endl;

        if (tokens.size() == 2) {
            if (tokens[0] == "PRINT") {
                commandList.push_back(std::make_shared<Command>(tokens[0], tokens[1], "", false));
            }
        }
        else if (tokens.size() == 3) {
            if (tokens[0] == "DECLARE" && isVariable(tokens[1]) && isValue(tokens[2])) {
                std::int16_t value = static_cast<std::int16_t>(std::stoi(tokens[2]));
                commandList.push_back(std::make_shared<Command>(tokens[0], tokens[1], value));
            }
            /*(string & commandT, bool isValue, std::string writeAddress, std::string writeVar, std::int16_t writeValue)*/
            else if (tokens[0] == "WRITE" && isMemoryAddressFormat(tokens[1])) {
                if (isValue(tokens[2])) {
                    std::int16_t value = static_cast<std::int16_t>(std::stoi(tokens[2]));
                    commandList.push_back(std::make_shared<Command>(tokens[0], true, tokens[1], "", value));
                }
                else if (isVariable(tokens[2])) {
                    commandList.push_back(std::make_shared<Command>(tokens[0], false, tokens[1], tokens[2], 0));
                }
            }
            else if (tokens[0] == "READ" && isVariable(tokens[1]) && isMemoryAddressFormat(tokens[2])) {
                commandList.push_back(std::make_shared<Command>(tokens[0], tokens[1], tokens[2]));
            }
        }
        else if (tokens.size() == 4) {
            /* (string commandT, uint8_t opMode, string varSum, string varAdd1, string varAdd2, int16_t opAdd1, int16_t opAdd2)*/
            if (tokens[0] == "ADD") {
                if (isVariable(tokens[1]) && isVariable(tokens[2]) && isVariable(tokens[3])) {
                    commandList.push_back(std::make_shared<Command>(tokens[0], 1, tokens[1], tokens[2], tokens[3], 0, 0));
                }
                else if (isVariable(tokens[1]) && isVariable(tokens[2]) && isValue(tokens[3])) {
                    std::int16_t value = static_cast<std::int16_t>(std::stoi(tokens[3]));
                    commandList.push_back(std::make_shared<Command>(tokens[0], 2, tokens[1], tokens[2], "", 0, value));
                }
                else if (isVariable(tokens[1]) && isValue(tokens[2]) && isVariable(tokens[3])) {
                    std::int16_t value = static_cast<std::int16_t>(std::stoi(tokens[2]));
                    commandList.push_back(std::make_shared<Command>(tokens[0], 3, tokens[1], "", tokens[3], value, 0));
                }
                else if (isVariable(tokens[1]) && isValue(tokens[2]) && isValue(tokens[3])) {
                    std::int16_t value1 = static_cast<std::int16_t>(std::stoi(tokens[2]));
                    std::int16_t value2 = static_cast<std::int16_t>(std::stoi(tokens[3]));
                    commandList.push_back(std::make_shared<Command>(tokens[0], 4, tokens[1], "", "", value1, value2));
                }
            }
            else if (tokens[0] == "SUBTRACT") {
                if (isVariable(tokens[1]) && isVariable(tokens[2]) && isVariable(tokens[3])) {
                    commandList.push_back(std::make_shared<Command>(tokens[0], 1, tokens[1], tokens[2], tokens[3], 0, 0));
                }
                else if (isVariable(tokens[1]) && isVariable(tokens[2]) && isValue(tokens[3])) {
                    std::int16_t value = static_cast<std::int16_t>(std::stoi(tokens[3]));
                    commandList.push_back(std::make_shared<Command>(tokens[0], 2, tokens[1], tokens[2], "", 0, value));
                }
                else if (isVariable(tokens[1]) && isValue(tokens[2]) && isVariable(tokens[3])) {
                    std::int16_t value = static_cast<std::int16_t>(std::stoi(tokens[2]));
                    commandList.push_back(std::make_shared<Command>(tokens[0], 3, tokens[1], "", tokens[3], value, 0));
                }
                else if (isVariable(tokens[1]) && isValue(tokens[2]) && isValue(tokens[3])) {
                    std::int16_t value1 = static_cast<std::int16_t>(std::stoi(tokens[2]));
                    std::int16_t value2 = static_cast<std::int16_t>(std::stoi(tokens[3]));
                    commandList.push_back(std::make_shared<Command>(tokens[0], 4, tokens[1], "", "", value1, value2));
                }
            }
            else if (tokens[0] == "PRINT") {
                if (tokens[2] == "+" && isVariable(tokens[3])) {
                    commandList.push_back(std::make_shared<Command>(tokens[0], tokens[1], tokens[3], true));
                }
            }
        }
        else {
            commandList.push_back(std::make_shared<Command>("placeholder", true));
        }
    }
}

bool Process::isValue(std::string token)
{
    try {
        int value = std::stoi(token);
        return value >= INT16_MIN && value <= INT16_MAX;
    }
    catch (...) {
        return false;
    }
}

bool Process::isMemoryAddressFormat(std::string token)
{
    if (token.size() < 3) return false;
    if (token[0] != '0' || token[1] != 'x') return false;

    for (std::size_t i = 2; i < token.size(); ++i) {
        if (!isxdigit(token[i])) return false;
    }

    return true;
}


bool Process::isVariable(std::string token)
{
    if (token.empty()) return false;
    if (!std::isalpha(token[0]) && token[0] != '_') return false;

    for (char c : token) {
        if (!std::isalnum(c) && c != '_') return false;
    }

    return true;
}

void Process::viewSymbolTableAndMemorySpace()
{
    std::cout << "=== SYMBOL TABLE ===" << std::endl;
    for (const auto& [var, value] : symbolTable) {
        std::cout << "Variable: " << var << " | Value: " << value << std::endl;
    }

    std::cout << "\n=== VIRTUAL MEMORY SPACE ===" << std::endl;
    for (const auto& [address, value] : virtualMemorySpace) {
        std::cout << "Address: " << address << " | Value: " << value << std::endl;
    }

    std::cout << "============================" << std::endl;
}

void Process::logPrintStatements(std::string print)
{
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm timeinfo;
    localtime_s(&timeinfo, &now_c);  // safer version of localtime

    std::ostringstream oss;
    oss << "(" << std::put_time(&timeinfo, "%m/%d/%Y, %I:%M:%S %p") << ") ";
    oss << "Core:" << this->getCurrentCPUID() << " ";
    oss << "\"" << print << "\"";

    printLogs.push_back(oss.str());
}


void Process::viewLogPrintStatements()
{
    std::stringstream ss;
    ss << "Logs:\n";
    for (const auto& log : printLogs) {
        ss << log << "\n";
    }
    ss << "\n";
    std::cout << ss.str();
}

bool Process::isProcessStoppedDueToMemoryAccessError()
{
    return processStoppedDueToMemoryAccessError;
}

void Process::stopProcess(std::string memoryAddress)
{
    processStoppedDueToMemoryAccessError = true;
    invalidMemoryAccessError = memoryAddress;
    processStoppedOn = std::time(nullptr);
    setProcessState(Process::STOPPED);
}

std::string Process::getInvalidMemoryAccess()
{
    return invalidMemoryAccessError;
}

std::time_t Process::getProcessStoppedOn()
{
    return processStoppedOn;
}

std::uint32_t Process::randomProcessSize(std::uint32_t minProcessSize, std::uint32_t maxProcessSize)
{
    // Predefined power-of-two sizes from 64 to 65536
    std::vector<std::uint32_t> validSizes = {
        64, 128, 256, 512, 1024,
        2048, 4096, 8192, 16384, 32768, 65536
    };

    // Filter the sizes within the min/max range
    std::vector<std::uint32_t> filtered;
    for (std::uint32_t size : validSizes) {
        if (size >= minProcessSize && size <= maxProcessSize) {
            filtered.push_back(size);
        }
    }

    if (filtered.empty()) {
        return minProcessSize; // Fallback
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(filtered.size() - 1));

    return filtered[dist(gen)];
}

std::uint32_t Process::getMemorySize()
{
    return processSize;
}

std::uint32_t Process::getFrameSize()
{
    return frameSize;
}

std::uint32_t Process::getTotalPages()
{
    return totalPages;
}

std::vector<std::vector<std::string>> Process::parseCompoundCommand(const std::string& compound)
{
    std::vector<std::vector<std::string>> parsedLines;
    std::string currentCommand;
    bool inQuotes = false;

    for (char ch : compound) {
        if (ch == '"') {
            inQuotes = !inQuotes;
        }

        if (ch == ';' && !inQuotes) {
            if (!currentCommand.empty()) {
                std::vector<std::string> tokens = tokenizeForCompound(currentCommand);
                if (!tokens.empty())
                    parsedLines.push_back(tokens);
                currentCommand.clear();
            }
        }
        else {
            currentCommand += ch;
        }
    }

    if (!currentCommand.empty()) {
        std::vector<std::string> tokens = tokenizeForCompound(currentCommand);
        if (!tokens.empty())
            parsedLines.push_back(tokens);
    }

    return parsedLines;
}

std::vector<std::string> Process::tokenizeForCompound(const std::string& command)
{
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;

    for (size_t i = 0; i < command.length(); ++i) {
        char ch = command[i];

        if (ch == '"') {
            inQuotes = !inQuotes;
            continue; // skip the quotes
        }

        if (inQuotes) {
            current += ch;
        }
        else if (ch == ' ' || ch == '(' || ch == ')') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            // skip the delimiter
        }
        else if (ch == '+') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back("+");
        }
        else {
            current += ch;
        }
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

std::string Process::cleanQuotedString(const std::string& str)
{
    std::string cleaned;

    // Remove leading and trailing quote if present
    size_t start = (str.front() == '"') ? 1 : 0;
    size_t end = (str.back() == '"') ? str.size() - 1 : str.size();

    for (size_t i = start; i < end; ++i) {
        if (str[i] == '\\' && i + 1 < end && str[i + 1] == '"') {
            cleaned += '"';
            ++i; // skip escaped quote
        }
        else {
            cleaned += str[i];
        }
    }

    return cleaned;
}

void Process::initializeVirtualMemoryMap()
{
    std::vector<std::string> memoryAddresses = {
        "0x100", "0x200", "0x300", "0x400", "0x500", "0x600", "0x700", "0x800", "0x900", "0x1000",
        "0x1100", "0x1200", "0x1300", "0x1400", "0x1500", "0x1600", "0x1700", "0x1800", "0x1900", "0x2000",
        "0x2100", "0x2200", "0x2300", "0x2400", "0x2500", "0x2600", "0x2700", "0x2800", "0x2900", "0x3000",
        "0x3100", "0x3200"
    };

    for (const auto& addrStr : memoryAddresses) {
        virtualMemorySpace[addrStr] = 0;
    }
}

void Process::insertValueToMemory(std::string memoryAddress, std::int16_t value)
{
    // Check if the memory address is valid
    if (virtualMemorySpace.find(memoryAddress) == virtualMemorySpace.end()) {
        stopProcess(memoryAddress);
        return;
    }

    virtualMemorySpace[memoryAddress] = value;
}

void Process::insertValueFromVarToMemory(std::string memoryAddress, std::string var)
{
    // Check if the memory address is valid
    if (virtualMemorySpace.find(memoryAddress) == virtualMemorySpace.end()) {
        stopProcess(memoryAddress);
        return;
    }


    // Check if the variable exists
    if (symbolTable.find(var) == symbolTable.end()) {
        return;
    }

    std::int16_t value = symbolTable[var];
    virtualMemorySpace[memoryAddress] = value;
}

void Process::insertValueFromAddressToVar(std::string var, std::string memoryAddress)
{
    // Check if memory address is valid
    if (virtualMemorySpace.find(memoryAddress) == virtualMemorySpace.end()) {
        stopProcess(memoryAddress);
        return;
    }

    // If variable doesn't exist and symbol table is full, don't insert
    if (symbolTable.find(var) == symbolTable.end() && symbolTable.size() >= 32) {
        return;
    }

    // Get value from memory and insert or update symbol table
    std::int16_t value = virtualMemorySpace[memoryAddress];
    symbolTable[var] = value;
}

void Process::insertValueToVar(std::string var, std::int16_t value)
{
    // Allow overwrite if variable exists, but restrict adding new if already at capacity
    if (symbolTable.find(var) == symbolTable.end() && symbolTable.size() >= 32) {
        return;
    }

    symbolTable[var] = value;
}


std::optional<std::int16_t> Process::getValueFromAddress(std::string memoryAddress) {
    if (virtualMemorySpace.find(memoryAddress) == virtualMemorySpace.end()) {
        stopProcess(memoryAddress);
        return std::nullopt;
    }

    if (virtualMemorySpace.find(memoryAddress) != virtualMemorySpace.end()) {
        return virtualMemorySpace[memoryAddress];
    }
    return std::nullopt;
}

std::optional<std::int16_t> Process::getValueFromVar(std::string var) {
    if (symbolTable.find(var) != symbolTable.end()) {
        return symbolTable[var];
    }
    return std::nullopt;
}



