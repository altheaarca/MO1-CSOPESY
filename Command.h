#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>

class Process;
enum class CommandType { PRINT, ADD, SUBTRACT, SLEEP, FOR_LOOP, DECLARE, READ, WRITE };

class Command {
public:
    using Arg = std::variant<std::string, uint16_t>;

    CommandType type;
    std::string message;

    // operands
    std::string targetVar;
    Arg operand1, operand2;
    uint8_t sleepTicks = 0;
    uint16_t repeatCount = 0;
    std::vector<Command> loopBody;

    // READ/WRITE args
    std::string readVar;
    uint32_t readAddress = 0;
    uint32_t writeAddress = 0;
    Arg writeValue;

    Command(CommandType t, const std::string& msg = "") : type(t), message(msg) {}

    void executeCommand(Process& proc, int cpuId);

    static Command makePrint(const std::string&);
    static Command makeAdd(const std::string&, Arg, Arg);
    static Command makeSubtract(const std::string&, Arg, Arg);
    static Command makeSleep(uint8_t);
    static Command makeForLoop(const std::vector<Command>&, uint16_t);
    static Command makeDeclare(const std::string&, uint16_t);
    static Command makeRead(const std::string&, uint32_t);
    static Command makeWrite(uint32_t, Arg);
};
