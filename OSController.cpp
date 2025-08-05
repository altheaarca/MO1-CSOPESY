#include "OSController.h"
#include "ConsoleManager.h"  // Needed for the member variable

OSController* OSController::instance = nullptr;

OSController* OSController::getInstance() {
    if (instance == nullptr) {
        instance = new OSController();
    }
    return instance;
}


void OSController::injectCoreComponents(std::shared_ptr<ConfigSpecs> config,
    std::shared_ptr<CommandManager> commandManager,
    std::shared_ptr<ProcessManager> processManager,
    std::shared_ptr<CPUScheduler> cpuScheduler,
    std::shared_ptr<MemoryManager> memoryManager,
    std::shared_ptr<BackingStore> backingStore) {
    this->config = std::move(config);
    this->commandManager = std::move(commandManager);
    this->processManager = std::move(processManager);
    this->cpuScheduler = std::move(cpuScheduler); // ? Set the scheduler
    this->memoryManager = std::move(memoryManager);
    this->backingStore = std::move(backingStore);
}

// Separate console manager setter
void OSController::setConsoleManager(std::shared_ptr<ConsoleManager> consoleManager) {
    this->consoleManager = std::move(consoleManager);
}

void OSController::setCPUScheduler(std::shared_ptr<CPUScheduler> cpuScheduler)
{
    this->cpuScheduler = std::move(cpuScheduler);
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

std::shared_ptr<CPUScheduler> OSController::getCPUScheduler()
{
    return cpuScheduler;
}

std::shared_ptr<MemoryManager> OSController::getMemoryManager()
{
    return memoryManager;
}

std::shared_ptr<BackingStore> OSController::getBackingStore()
{
    return backingStore;
}

// Initialization check & flag
bool OSController::isOSInitialized() const {
    return isInitialized;
}

void OSController::initialize() {
    isInitialized = true;
}
