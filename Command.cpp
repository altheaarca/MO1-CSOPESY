#include "Command.h"
#include "Process.h"   
#include "OSController.h"
#include <random>

Command::Command(const std::string& commandT)
{
    commandType = commandT;
}

Command::Command(const std::string& commandT, bool simulated)
    : commandType(commandT), simulated(simulated) {
}

Command::Command(const std::string& commandT, std::string declareVar, std::int16_t declareValue)
    : commandType(commandT),
    declareVar(declareVar),
    declareValue(declareValue) {
}

Command::Command(const std::string& commandT, std::uint8_t opMode, std::string varSum, std::string varAdd1, std::string varAdd2, std::int16_t opAdd1, std::int16_t opAdd2)
{
    if (commandT == "ADD") {
        commandType = commandT;
        this->opMode = opMode;
        this->varSum = varSum;
        this->varAdd1 = varAdd1;
        this->varAdd2 = varAdd2;
        this->opAdd1 = opAdd1;
        this->opAdd2 = opAdd2;
    }
    else if (commandT == "SUBTRACT") {
        commandType = commandT;
        this->opMode = opMode;
        this->varDiff = varSum;
        this->varSub1 = varAdd1;
        this->varSub2 = varAdd2;
        this->opSub1 = opAdd1;
        this->opSub2 = opAdd2;
    }
}

Command::Command(const std::string& commandT, bool isValue, std::string writeAddress, std::string writeVar, std::int16_t writeValue)
    : commandType(commandT),
    isValue(isValue),
    writeAddress(writeAddress),
    writeVar(writeVar),
    writeValue(writeValue) {
}

Command::Command(const std::string& commandT, std::string readVar, std::string readAddress)
    : commandType(commandT),
    readVar(readVar),
    readAddress(readAddress) {
}

Command::Command(const std::string& commandT, std::string printString, std::string printValueFromVar, bool withVar)
    : commandType(commandT),
    withVar(withVar),
    printString(printString),
    printValueFromVar(printValueFromVar) {
}

// Method used in your process execution — good
void Command::executeCommand() {
    /* std::cout << "Executing command (no process): " << commandType << std::endl;*/
}

void Command::executeCommand2(Process& proc) {
    if (simulated) {
        auto processSize = proc.getLinesOfCode();

        // Determine chance: start at 1/32, double every 1000 lines
        int baseChance = 32;
        int exponent = processSize / 1000;
        int chanceDenominator = baseChance * (1 << exponent); // 1 << exponent = 2^exponent

        // RNG setup
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, chanceDenominator - 1);

        // 1/N chance
        if (dist(gen) == 0) {
            std::uint32_t value = proc.increasingValue;
            std::string message = "Hello world from " + proc.getProcessName() + "!" + " With var x: " + std::to_string(value);
            proc.logPrintStatements(message);
            proc.increasingValue++;
        }
    }


    if (commandType == "DECLARE") {
        proc.insertValueToVar(declareVar, declareValue);
        /*proc.viewSymbolTableAndMemorySpace();*/
    }
    else if (commandType == "ADD") {
        if (opMode == 1) {
            auto var1 = proc.getValueFromVar(varAdd1);
            auto var2 = proc.getValueFromVar(varAdd2);
            if (var1.has_value() && var2.has_value()) {
                auto result = var1.value() + var2.value();
                proc.insertValueToVar(varSum, result);
            }
        }
        if (opMode == 2) {
            auto var1 = proc.getValueFromVar(varAdd1);
            if (var1.has_value()) {
                auto result = var1.value() + opAdd2;
                proc.insertValueToVar(varSum, result);
            }
        }
        if (opMode == 3) {
            auto var2 = proc.getValueFromVar(varAdd2);
            if (var2.has_value()) {
                auto result = opAdd1 + var2.value();
                proc.insertValueToVar(varSum, result);
            }
        }
        if (opMode == 4) {
            auto result = opAdd1 + opAdd2;
            proc.insertValueToVar(varSum, result);
        }
    }
    else if (commandType == "SUBTRACT") {
        if (opMode == 1) {
            auto var1 = proc.getValueFromVar(varSub1);
            auto var2 = proc.getValueFromVar(varSub2);
            if (var1.has_value() && var2.has_value()) {
                auto result = var1.value() - var2.value();
                proc.insertValueToVar(varDiff, result);
            }
        }
        if (opMode == 2) {
            auto var1 = proc.getValueFromVar(varSub1);
            if (var1.has_value()) {
                auto result = var1.value() - opSub2;
                proc.insertValueToVar(varDiff, result);
            }
        }
        if (opMode == 3) {
            auto var2 = proc.getValueFromVar(varSub2);
            if (var2.has_value()) {
                auto result = opSub1 - var2.value();
                proc.insertValueToVar(varDiff, result);
            }
        }
        if (opMode == 4) {
            auto result = opSub1 - opSub2;
            proc.insertValueToVar(varDiff, result);
        }
    }
    else if (commandType == "WRITE") {
        if (isValue) {
            proc.insertValueToMemory(writeAddress, writeValue);
        }
        else {
            proc.insertValueFromVarToMemory(writeAddress, writeVar);
        }
    }
    else if (commandType == "READ") {
        proc.insertValueFromAddressToVar(readVar, readAddress);
    }
    else if (commandType == "PRINT") {
        if (withVar) {
            std::string pString = printString;
            auto value = proc.getValueFromVar(printValueFromVar);
            std::int16_t printValue = value.value();
            proc.logPrintStatements(pString + std::to_string(printValue));
        }
        else {
            std::string pString = printString;
            proc.logPrintStatements(pString);
        }
    }
}

const std::string& Command::getCommandType() const {
    return commandType;
}
