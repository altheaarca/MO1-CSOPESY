#pragma once
#include <string>

#include "OPESY/BaseConsole.h"
class customizedLayout :public BaseConsole {
public:
    customizedLayout(const std::string &screenName);
    void runConsole() override;
};


