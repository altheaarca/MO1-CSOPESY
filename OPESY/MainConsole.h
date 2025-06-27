#pragma once

#include "../MO1 - CSOPESY/OPESY/BaseConsole.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class MainConsole : public BaseConsole {
public:
    void runConsole() override;

private:
    std::vector<std::string> tokenizeInput(const std::string& input); 
};
