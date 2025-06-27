#include "ProcessConsole.h"
#include "OSController.h"

ProcessConsole::ProcessConsole(std::string screenName, std::shared_ptr<Process> process)
{
	processScreenName = screenName;
	attachedProcess = process;
}

void ProcessConsole::runConsole()
{
	std::cout << "Welcome to " << processScreenName << std::endl << std::endl;

	isProcessConsoleRunning = true;
	std::string input;

	while (isProcessConsoleRunning) {
		std::cout << ">> ";
		std::getline(std::cin, input);

		if (input.empty())
			continue;

		if (input == "exit") {
			isProcessConsoleRunning = false;
			/*OSController::getInstance()->getConsoleManager()->clearScreen();*/
		}
		if (input == "1" || input == "process-smi") {
			if (!attachedProcess->isFinished()) {
				std::cout << "Process name: " << attachedProcess->getProcessName() << "\n";
				std::cout << "ID: " << attachedProcess->getProcessID() << "\n";
				std::cout << "Current instruction line: " << attachedProcess->getCurrentInstructionLine() << "\n";
				std::cout << "Lines of code: " << attachedProcess->getLinesOfCode() << "\n";
				std::time_t created = attachedProcess->getProcessCreatedOn();
				std::tm timeinfo{};
				localtime_s(&timeinfo, &created);

				std::cout << "Created On: ("
					<< std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p")
					<< ")\n";

				std::cout << "State: " << processStateToString(attachedProcess->getProcessState()) << "\n";
				std::cout << "CPU ID: " << attachedProcess->getCurrentCPUID() << "\n\n";
			}
			else {
				std::cout << "Process name: " << attachedProcess->getProcessName() << "\n";
				std::cout << "ID: " << attachedProcess->getProcessID() << "\n";
				std::time_t finished = attachedProcess->getProcessFinishedOn();
				std::tm timeinfo{};
				localtime_s(&timeinfo, &finished);

				std::cout << "Created On: ("
					<< std::put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p")
					<< ")\n";

				std::cout << "State: " << processStateToString(attachedProcess->getProcessState()) << "\n";
				std::cout << "CPU ID: " << attachedProcess->getCurrentCPUID() << "\n";
				std::cout << "Finished! " << "\n\n";
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