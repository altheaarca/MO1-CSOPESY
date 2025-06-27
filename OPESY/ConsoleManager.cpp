#include "ConsoleManager.h"


ConsoleManager::ConsoleManager()
{
}

void ConsoleManager::CSOPESYHeader()
{
	const std::string COLOR_GREEN = "\033[32m";
	const std::string COLOR_YELLOW = "\033[93m";
	const std::string COLOR_PINK = "\033[35m"; 
	const std::string COLOR_RESET = "\033[0m";

	std::cout << COLOR_PINK;
	std::cout << R"( 
 _______  _______  _______  _______  _______  _______			
(  ____ \(  ____ \(  ___  )(  ____ )(  ____ \(  ____ \|\     /|
| (    \/| (    \/| (   ) || (    )|| (    \/| (    \/( \   / )
| |      | (_____ | |   | || (____)|| (__    | (_____  \ (_) / 
| |      (_____  )| |   | ||  _____)|  __)   (_____  )  \   /  
| |            ) || |   | || (      | (            ) |   ) (   
| (____/\/\____) || (___) || )      | (____/\/\____) |   | |   
(_______/\_______)(_______)|/       (_______/\_______)   \_/     
)";
	std::cout << COLOR_RESET;
	std::cout << "\n ==========================" << std::endl;
	std::cout << "\n Hello, Welcome to CSOPESY commandline!\n" << COLOR_GREEN;
	std::cout << "\n Developer: Arca, Althea Denisse\n" << COLOR_RESET;
	std::cout << "\n Last Updated: 06-25-2025 \n" << COLOR_RESET;
	std::cout << "\n ==========================\n " << std::endl;
	std::cout << COLOR_YELLOW << "Type 'exit' to quit, 'clear' to clear the screen\n" << COLOR_YELLOW;
	std::cout << COLOR_RESET;
}

void ConsoleManager::clearScreen()
{
	system("cls");
	CSOPESYHeader();
}

void ConsoleManager::createProcessConsole(const std::string& name, std::shared_ptr<BaseConsole> console) {
	consoleMap[name] = console;
}

std::shared_ptr<BaseConsole> ConsoleManager::getConsole(const std::string& name) {
	auto it = consoleMap.find(name);
	if (it != consoleMap.end()) {
		return it->second;
	}
	return nullptr;
}

bool ConsoleManager::processConsoleExists(const std::string& name) const {
	return consoleMap.find(name) != consoleMap.end();
}

void ConsoleManager::switchToProcessConsole(const std::string& name) {
	auto it = consoleMap.find(name);
	if (it != consoleMap.end()) {
		currentConsole = it->second;
		system("cls");
		currentConsole->runConsole();  
		clearScreen();
	}
	else {
		std::cout << "Process console '" << name << "' not found.\n\n";
	}
}

void ConsoleManager::listProcessConsoles()
{
	std::cout << "=== Registered Process Consoles ===\n";
	for (const auto& [name, console] : consoleMap) {
		auto procConsole = std::dynamic_pointer_cast<ProcessConsole>(console);
		if (procConsole) {
			std::cout << "- " << procConsole->getProcessScreenName() << '\n';
		}
	}
	std::cout << std::endl;
}

int ConsoleManager::getGlobalProcessID()
{
	return globalProcessID;
}

void ConsoleManager::incrementGlobalProcessID()
{
	globalProcessID++;
}

 