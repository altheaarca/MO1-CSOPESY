#pragma once

class BaseConsole {
public:
    virtual ~BaseConsole() = default;
    virtual void runConsole() = 0;
};
