#pragma once
#include <iostream>

class Command {
public:
    ~Command() = default;

    void executeCommand();
};
