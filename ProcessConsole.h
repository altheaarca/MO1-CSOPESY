#pragma once
#include "../MO1 - CSOPESY/OPESY/BaseConsole.h"
#include "Process.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <ctime> 

class ProcessConsole : public BaseConsole {
public:
	ProcessConsole(std::string screenName, std::shared_ptr<Process> process);
	void nvidiadummy();
	void runConsole() override;
	std::string getProcessScreenName() const;
	std::string processStateToString(Process::ProcessState state);
private:
	std::shared_ptr<Process> attachedProcess;
	bool isProcessConsoleRunning = false;
	std::string processScreenName;
};

