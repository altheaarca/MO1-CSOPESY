#pragma once
#include <string>
#include <vector>
#include <variant>

class Process; // forward declaration

class Command {
public:
    enum CommandType {
        PRINT,
        ADD,
        SUBTRACT,
        SLEEP,
        FOR_LOOP
    };

    using Arg = std::variant<std::string, uint16_t>; // for var or constant

    Command(CommandType type = PRINT, const std::string& msg = "");

    // Overloads for various commands
    static Command makePrint(const std::string& msg);
    static Command makeAdd(const std::string& var, Arg a, Arg b);
    static Command makeSubtract(const std::string& var, Arg a, Arg b);
    static Command makeSleep(uint8_t ticks);
    static Command makeForLoop(const std::vector<Command>& instructions, uint16_t repeatCount);

    void executeCommand(const std::string& processName, int cpuId);
    void executeCommand(Process& proc, int cpuId); // for stateful ops

    CommandType getType() const;

private:
    CommandType type;
    std::string message;

    // For ADD/SUBTRACT
    std::string targetVar;
    Arg operand1, operand2;

    // For SLEEP
    uint8_t sleepTicks = 0;

    // For FOR_LOOP
    std::vector<Command> loopBody;
    uint16_t repeatCount = 0;
};
