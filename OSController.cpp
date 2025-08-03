#include "OSController.h"
#include "ConsoleManager.h"  // Needed for the member variable
#include "CPUScheduler.h"

OSController* OSController::instance = nullptr;

OSController* OSController::getInstance() {
    if (instance == nullptr) {
        instance = new OSController();
    }
    return instance;
}
// Unified component injection
void OSController::injectCoreComponents(std::shared_ptr<ConfigSpecs> config, std::shared_ptr<CommandManager> commandManager, 
    std::shared_ptr<ProcessManager> processManager) {
    this->config = config; 
    this->commandManager = std::move(commandManager);
    this->processManager = std::move(processManager);

}

// Separate console manager setter
void OSController::setConsoleManager(std::shared_ptr<ConsoleManager> consoleManager) {
    this->consoleManager = std::move(consoleManager);
}

// Accessors
std::shared_ptr<ConfigSpecs> OSController::getConfig() {
    return config;
}

std::shared_ptr<ConsoleManager> OSController::getConsoleManager() {
    return consoleManager;
}

std::shared_ptr<CommandManager> OSController::getCommandManager()
{
    return commandManager;
}

std::shared_ptr<ProcessManager> OSController::getProcessManager()
{
    return processManager;
}

// Initialization check & flag
bool OSController::isOSInitialized() const {
    return isInitialized;
}

void OSController::initialize() {
    isInitialized = true;
}
void OSController::setCPUScheduler(std::shared_ptr<CPUScheduler> scheduler) {
    this->cpuScheduler = scheduler;
}
std::shared_ptr<CPUScheduler> OSController::getCPUScheduler() {
	// Check if the CPU scheduler is initialized. nullptr check is not needed here as the method will return nullptr if not set.
    auto scheduler = OSController::getInstance()->getCPUScheduler();
    if (scheduler)
        scheduler->printReport(std::cout);
    else
        std::cout << "Scheduler not initialized.\n";
	// Return the CPU scheduler instance
    return cpuScheduler;
}
OSController::~OSController() {
    std::cout << "[OSController] Destroyed.\n";
}



