#include "ProcessConsole.h"
#include "OSController.h"

ProcessConsole::ProcessConsole(std::string screenName, std::shared_ptr<Process> process)
{
	processScreenName = screenName;
	attachedProcess = process;
}

void ProcessConsole::runConsole()
{
	auto config = OSController::getInstance()->getConfig();
	std::cout << "Welcome to " << processScreenName << std::endl << std::endl;
	std::cout << "Process Memory Size: " << attachedProcess->getMemorySize() << std::endl;
	std::cout << "Pages Required: " << attachedProcess->getTotalPages() << std::endl;
	std::cout << "OS Memory Frame Size: " << config->getMemPerFrame() << std::endl << std::endl;

	std::cout << "Current instruction line: " << attachedProcess->getCurrentInstructionLine() << "\n";
	std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n";

	attachedProcess->viewLogPrintStatements();
	isProcessConsoleRunning = true;
	std::string input;

	while (isProcessConsoleRunning) {
		std::cout << ">> ";
		std::getline(std::cin, input);

		if (input.empty())
			continue;

		if (input == "exit") {
			isProcessConsoleRunning = false;
		}
		if (input == "1" || input == "process-smi") {
			if (!attachedProcess->isFinished()) {
				std::cout << "\nProcess name: " << attachedProcess->getProcessName() << "\n";
				std::cout << "ID: " << attachedProcess->getProcessID() << "\n";
				attachedProcess->viewLogPrintStatements();
				std::cout << "Current instruction line: " << attachedProcess->getCurrentInstructionLine() << "\n";
				std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n";
				std::time_t created = attachedProcess->getProcessCreatedOn();
				std::tm timeinfo{};
				localtime_s(&timeinfo, &created);

				std::cout << "Created On: ("
					<< std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p")
					<< ")\n\n";

				/*		std::cout << "State: " << processStateToString(attachedProcess->getProcessState()) << "\n";*/
			/*			std::cout << "CPU ID: " << attachedProcess->getCurrentCPUID() << "\n\n";*/
			}
			if (attachedProcess->isProcessStoppedDueToMemoryAccessError()) {
				std::cout << "\nProcess name: " << attachedProcess->getProcessName() << "\n";
				std::cout << "ID: " << attachedProcess->getProcessID() << "\n";
				attachedProcess->viewLogPrintStatements();
				std::cout << "Current instruction line: " << attachedProcess->getCurrentInstructionLine() << "\n";
				std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n";
				std::time_t created = attachedProcess->getProcessCreatedOn();
				std::tm timeinfo{};
				localtime_s(&timeinfo, &created);

				std::cout << "Created On: ("
					<< std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p")
					<< ")\n\n";


				time_t stopTime = attachedProcess->getProcessStoppedOn();

				std::tm timeInfo;
				localtime_s(&timeInfo, &stopTime);
				std::ostringstream oss;
				oss << "" << std::put_time(&timeInfo, "%H:%M:%S") << "";
				std::string formattedTime = oss.str();

				std::cout << "Memory access violation error at address " << attachedProcess->getInvalidMemoryAccess() << " (" << formattedTime << ")\n\n";

				/*		std::cout << "State: " << processStateToString(attachedProcess->getProcessState()) << "\n";*/
			/*			std::cout << "CPU ID: " << attachedProcess->getCurrentCPUID() << "\n\n";*/
			}
			if (attachedProcess->isFinished()) {
				std::cout << "\nProcess name: " << attachedProcess->getProcessName() << "\n";
				std::cout << "ID: " << attachedProcess->getProcessID() << "\n";
				attachedProcess->viewLogPrintStatements();
				std::time_t finished = attachedProcess->getProcessFinishedOn();
				std::tm timeinfo{};
				localtime_s(&timeinfo, &finished);

				std::cout << "Finished On: ("
					<< std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p")
					<< ")\n\n";

				/*		std::cout << "State: " << processStateToString(attachedProcess->getProcessState()) << "\n";*/
				/*		std::cout << "CPU ID: " << attachedProcess->getCurrentCPUID() << "\n";*/
						//std::cout << "Finished! " << "\n\n";
						/*attachedProcess->viewSymbolTableAndMemorySpace();*/
						//attachedProcess->viewLogPrintStatements();
			}
		}
		else {
			std::cout << "No commands yet." << std::endl << std::endl;
		}
	}
}

std::string ProcessConsole::getProcessScreenName() const
{
	return processScreenName;
}

std::string ProcessConsole::processStateToString(Process::ProcessState state) {
	switch (state) {
	case Process::NEW: return "NEW";
	case Process::RUNNING: return "RUNNING";
	case Process::PRE_EMPTED: return "PRE_EMPTED";
	case Process::FINISHED: return "FINISHED";
	default: return "UNKNOWN";
	}
}

std::shared_ptr<Process> ProcessConsole::getAttachedProcess()
{
	return attachedProcess;
}
