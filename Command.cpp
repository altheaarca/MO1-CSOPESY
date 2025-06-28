#include "Command.h"

#include <algorithm>

#include "Process.h"
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <thread>
#include <unordered_map>


namespace {
    uint16_t resolveArg(const Command::Arg& arg, const std::unordered_map<std::string, uint16_t>& vars) {
        if (std::holds_alternative<std::string>(arg)) {
            auto it = vars.find(std::get<std::string>(arg));
            return (it != vars.end()) ? it->second : 0;
        }
        return std::get<uint16_t>(arg);
    }
}

Command::Command(CommandType type, const std::string& msg)
    : type(type), message(msg) {
}

void Command::executeCommand(Process& proc, int cpuId) {
    auto& vars = proc.getVariables();

    switch (type) {
    case PRINT: {
        executeCommand(proc.getProcessName(), cpuId);
        break;
    }

    case ADD: {
        uint16_t val1 = resolveArg(operand1, vars);
        uint16_t val2 = resolveArg(operand2, vars);
        vars[targetVar] = static_cast<uint16_t>(std::clamp<int>(val1 + val2, 0, 65535));
        break;
    }

    case SUBTRACT: {
        uint16_t val1 = resolveArg(operand1, vars);
        uint16_t val2 = resolveArg(operand2, vars);
        vars[targetVar] = static_cast<uint16_t>(std::clamp<int>(val1 - val2, 0, 65535));
        break;
    }

    case SLEEP: {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTicks));
        break;
    }

    case FOR_LOOP: {
        for (int i = 0; i < repeatCount; ++i) {
            for (auto& cmd : loopBody) {
                cmd.executeCommand(proc, cpuId);
            }
        }
        break;
    }

    default:
        break;
    }
}

void Command::executeCommand(const std::string& processName, int cpuId) {
    if (type == PRINT) {
        std::ofstream file(processName + ".txt", std::ios::app);
        if (!file.is_open()) return;

        auto now = std::chrono::system_clock::now();
        std::time_t execTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTime{};
        localtime_s(&localTime, &execTime);

        std::ostringstream timestamp;
        timestamp << std::put_time(&localTime, "%m/%d/%Y %I:%M:%S %p");

        file << "[" << timestamp.str() << "] [CPU " << cpuId << "] Hello world from " << processName << "!\n";
    }
}



Command Command::makePrint(const std::string& msg) {
    return Command(PRINT, msg);
}

Command Command::makeAdd(const std::string& var, Arg a, Arg b) {
    Command cmd(ADD);
    cmd.targetVar = var;
    cmd.operand1 = a;
    cmd.operand2 = b;
    return cmd;
}

Command Command::makeSubtract(const std::string& var, Arg a, Arg b) {
    Command cmd(SUBTRACT);
    cmd.targetVar = var;
    cmd.operand1 = a;
    cmd.operand2 = b;
    return cmd;
}

Command Command::makeSleep(uint8_t ticks) {
    Command cmd(SLEEP);
    cmd.sleepTicks = ticks;
    return cmd;
}

Command Command::makeForLoop(const std::vector<Command>& instructions, uint16_t count) {
    Command cmd(FOR_LOOP);
    cmd.loopBody = instructions;
    cmd.repeatCount = count;
    return cmd;
}

Command::CommandType Command::getType() const {
    return type;
}
