#pragma once

#include <memory>
#include "ConfigSpecs.h"
#include "ConsoleManager.h"
#include "CommandManager.h"
#include "ProcessManager.h"

class OSController {
public:
    static OSController* getInstance();

    void injectCoreComponents(std::shared_ptr<ConfigSpecs> config, std::shared_ptr<CommandManager> commandManager,
        std::shared_ptr<ProcessManager> processManager);
    void setConsoleManager(std::shared_ptr<ConsoleManager> consoleManager);

    std::shared_ptr<ConfigSpecs> getConfig();
    std::shared_ptr<ConsoleManager> getConsoleManager();
    std::shared_ptr<CommandManager> getCommandManager();
    std::shared_ptr<ProcessManager> getProcessManager();

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
};
