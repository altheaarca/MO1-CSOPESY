// Command.cpp  (excerpt with paging hooks)
#include "Command.h"
#include "Process.h"
#include "OSController.h"
#include "MemoryManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <variant>
#include <thread>
#include <chrono>

namespace {
    uint16_t resolveArg(const Command::Arg& arg, const auto& vars) {
        if (std::holds_alternative<std::string>(arg)) {
            auto it = vars.find(std::get<std::string>(arg));
            return it != vars.end() ? it->second : 0;
        }
        return std::get<uint16_t>(arg);
    }
}

void Command::executeCommand(Process& proc, int cpuId) {
    auto& vars = proc.getVariables();
    auto mm = OSController::getInstance()->getCPUScheduler()->getMemoryManager();

    switch (type) {
    case CommandType::DECLARE: {
        // symbol-table page 0
        if (!proc.isPageValid(0)) {
            mm->loadPage(proc.getProcessName(), 0, false);
            proc.setPageValid(0,
                proc.getFrameForPage(0),
                mm->getCpuCycles());
        }
        // declare var with 0 if absent
        vars[targetVar] = vars[targetVar];
        break;
    }
    case CommandType::READ: {
        int page = (int)(readAddress / mm->getFrameSize());
        if (!proc.isPageValid(page)) {
            mm->loadPage(proc.getProcessName(), page, false);
            proc.setPageValid(page,
                proc.getFrameForPage(page),
                mm->getCpuCycles());
        }
        // simulate read
        uint16_t val = 0;
        vars[readVar] = val;
        break;
    }
    case CommandType::WRITE: {
        int page = (int)(writeAddress / mm->getFrameSize());
        if (!proc.isPageValid(page)) {
            mm->loadPage(proc.getProcessName(), page, true);
            proc.setPageValid(page,
                proc.getFrameForPage(page),
                mm->getCpuCycles());
        }
        proc.pageTable[page].modified = true;

        break;
    }
    case CommandType::PRINT: {
        std::ofstream out(proc.getProcessName() + ".txt", std::ios::app);
        if (!out) break;
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tm{}; localtime_s(&tm, &now);
        out << "[" << std::put_time(&tm, "%m/%d/%Y %I:%M:%S %p")
            << "] [CPU " << cpuId << "] " << message << "\n";
        break;
    }
    case CommandType::ADD: {
        uint16_t a = resolveArg(operand1, vars), b = resolveArg(operand2, vars);
        vars[targetVar] = (uint16_t)std::clamp<int>(a + b, 0, 65535);
        break;
    }
    case CommandType::SUBTRACT: {
        uint16_t a = resolveArg(operand1, vars), b = resolveArg(operand2, vars);
        vars[targetVar] = (uint16_t)std::clamp<int>(a - b, 0, 65535);
        break;
    }
    case CommandType::SLEEP: {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTicks));
        break;
    }
    case CommandType::FOR_LOOP: {
        for (int i = 0; i < repeatCount; ++i) {
            for (auto& cmd : loopBody) {
                cmd.executeCommand(proc, cpuId);
            }
        }
        break;
    }
    }
}
