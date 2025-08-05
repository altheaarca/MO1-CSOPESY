#pragma once

#include <memory>
#include "ConfigSpecs.h"
#include "ConsoleManager.h"
#include "CommandManager.h"
#include "ProcessManager.h"
#include "CPUScheduler.h"
#include "MemoryManager.h"
#include "BackingStore.h"

class OSController {
public:
    static OSController* getInstance();

    void injectCoreComponents(std::shared_ptr<ConfigSpecs> config,
        std::shared_ptr<CommandManager> commandManager,
        std::shared_ptr<ProcessManager> processManager,
        std::shared_ptr<CPUScheduler> cpuScheduler,
        std::shared_ptr<MemoryManager> memoryManager,
        std::shared_ptr<BackingStore> backingStore); // ? Add this

    void setConsoleManager(std::shared_ptr<ConsoleManager> consoleManager);
    void setCPUScheduler(std::shared_ptr<CPUScheduler> cpuScheduler);

    std::shared_ptr<ConfigSpecs> getConfig();
    std::shared_ptr<ConsoleManager> getConsoleManager();
    std::shared_ptr<CommandManager> getCommandManager();
    std::shared_ptr<ProcessManager> getProcessManager();
    std::shared_ptr<CPUScheduler> getCPUScheduler();
    std::shared_ptr<MemoryManager> getMemoryManager();
    std::shared_ptr<BackingStore> getBackingStore();

    bool isOSInitialized() const;
    void initialize();

private:
    OSController() = default;
    ~OSController() = default;

    OSController(const OSController&) = delete;
    OSController& operator=(const OSController&) = delete;

    static OSController* instance;

    bool isInitialized = false;

    std::shared_ptr<ConfigSpecs> config;
    std::shared_ptr<ConsoleManager> consoleManager;
    std::shared_ptr<CommandManager> commandManager;
    std::shared_ptr<ProcessManager> processManager;
    std::shared_ptr<CPUScheduler> cpuScheduler;
    std::shared_ptr<MemoryManager> memoryManager;
    std::shared_ptr<BackingStore> backingStore;
};
