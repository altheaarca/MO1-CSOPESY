#include "customizedLayout.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include "OSController.h"

struct ProcessInfo {
    int pid;
    std::string type;           \
    std::string name;
    std::string memoryUsage;            
};
std::vector<ProcessInfo> dummyProcesses;


customizedLayout::customizedLayout(const std::string& screenName) {

}

void customizedLayout::runConsole() {

		std::vector<ProcessInfo> dummyProcesses;

		dummyProcesses.push_back({ 1234, "C", "dummy_proc1", "1500 MiB" });
		dummyProcesses.push_back({ 1235, "G", "dummy_proc2", "1800 MiB" });
		dummyProcesses.push_back({ 1236, "G", "dummy_proc3", "1100 MiB" });
		dummyProcesses.push_back({ 1237, "C", "dummy_proc4", "1300 MiB" });
		dummyProcesses.push_back({ 1238, "C", "dummy_proc5", "1700 MiB" });
		std::cout << R"( 
		+-----------------------------------------------------------------------------------------+
		| NVIDIA-SMI 550.54.15              Driver Version: 550.54.15      CUDA Version: 12.4     |
		|-----------------------------------------+------------------------+----------------------+
		| GPU  Name                 Persistence-M | Bus-Id          Disp.A | Volatile Uncorr. ECC |
		| Fan  Temp   Perf          Pwr:Usage/Cap |           Memory-Usage | GPU-Util  Compute M. |
		|                                         |                        |               MIG M. |
		|=========================================+========================+======================|
		|   0  Tesla T4                       Off |   00000000:00:04.0 Off |                    0 |
		| N/A   55C    P8             10W /   70W |       0MiB /  15360MiB |      0%      Default |
		|                                         |                        |                  N/A |
		+-----------------------------------------+------------------------+----------------------+  
	)";
	std::cout << "        +-----------------------------------------------------------------------------------------+\n";
	std::cout << "		| Processes:                                                                              |\n";
	std::cout << "		|  GPU   GI   CI        PID   Type		Process name		       GPU Memory |\n";
	std::cout << "		|        ID   ID                                                               Usage      |\n";
	std::cout << "		|=========================================================================================|\n";

	for (const auto& proc : dummyProcesses) {
    std::cout << "		|" << std::setw(4) << 0
              << "  " << std::setw(3) << "N/A"
              << "  " << std::setw(3) << "N/A"
              <<"    " << std::setw(8) << proc.pid
              << "  " << std::setw(5) << proc.type
              <<"" << std::setw(25) << proc.name
              << "   " << std::setw(25) << proc.memoryUsage
              << "	  |\n";
	}
	std::cout << "		+-----------------------------------------------------------------------------------------+\n";
	std::cout << "\nPress Enter to return...";
	std::string dummy;
	std::getline(std::cin, dummy);
	OSController::getInstance()->getConsoleManager()->clearScreen();

}



