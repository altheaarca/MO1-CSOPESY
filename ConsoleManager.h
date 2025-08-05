#pragma once

#include <memory>
#include <string> 
#include <vector>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <unordered_map>
#include "BaseConsole.h"
#include "ProcessConsole.h"

class ConsoleManager
{
public:
    explicit ConsoleManager();
    void CSOPESYHeader(); //ASCII
    void clearScreen();
    void createProcessConsole(const std::string& name, std::shared_ptr<BaseConsole> console); // screen -s
    std::shared_ptr<BaseConsole> getConsole(const std::string& name); //gets a console by name
    bool processConsoleExists(const std::string& name) const; // checker if nageexist console
    void switchToProcessConsole(const std::string& name); // pang screen -r 
    void listProcessConsoles(); // pang screen -ls 
    int getGlobalProcessID(); //PID getter 
    void incrementGlobalProcessID(); //global PID incrementer
	std::shared_ptr<ProcessConsole> getProcessConsole(const std::string& name);
private:
    std::unordered_map<std::string, std::shared_ptr<BaseConsole>> consoleMap;
    std::shared_ptr<BaseConsole> currentConsole;
    int globalProcessID = 0;
};

