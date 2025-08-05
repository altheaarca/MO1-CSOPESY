#pragma once
#include <string>

class Process;

class Command {
public:
    // Add, Declare, For, Print, Read, Sleep, Subtract, Write
    Command(const std::string& commandT);
    Command(const std::string& commandT, bool simulated);
    // Declare
    Command(const std::string& commandT, std::string declareVar, std::int16_t declareValue);
    Command(const std::string& commandT, std::uint8_t opMode, std::string varSum, std::string varAdd1, std::string varAdd2, std::int16_t opAdd1, std::int16_t opAdd2);
    /*Command(const std::string& commandT, std::uint8_t opMode, std::string varDiff, std::string varSub1, std::string varSub2, std::int16_t opSub1, std::int16_t opSub2);*/
    Command(const std::string& commandT, bool isValue, std::string writeAddress, std::string writeVar, std::int16_t writeValue);
    Command(const std::string& commandT, std::string readVar, std::string readAddress);
    Command(const std::string& commandT, std::string printString, std::string printValueFromVar, bool withVar);


    void executeCommand();
    void executeCommand2(Process& proc);
    const std::string& getCommandType() const;

private:
    std::uint32_t increasingValue = 0;
    bool simulated = false;
    std::string commandType = "";
    // declare var value
    std::string declareVar = "";
    std::int16_t declareValue = 0;

    std::string varSum = "";

    std::string varAdd1 = "";
    std::string varAdd2 = "";
    std::int16_t opAdd1 = 0;
    std::int16_t opAdd2 = 0;

    std::string varDiff = "";
    std::string varSub1 = "";
    std::string varSub2 = "";
    std::int16_t opSub1 = 0;
    std::int16_t opSub2 = 0;

    // write address var
    // write address value
    std::string writeAddress = "";
    std::string writeVar = "";
    std::int16_t writeValue = 0;

    // read var address;
    std::string readVar = "";
    std::string readAddress = "";

    // print string var
    std::string printString = "";
    std::string printValueFromVar = "";

    std::uint8_t opMode = 0;
    bool isValue = false;
    bool withVar = false;
};
