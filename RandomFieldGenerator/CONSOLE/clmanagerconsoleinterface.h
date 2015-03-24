#ifndef CLMANAGERCONSOLEINTERFACE
#define CLMANAGERCONSOLEINTERFACE

#include "console.h"

#include "CLMANAGER/clmanager.h"
#include "consolecommand.h"

namespace Controller
{
class CLManagerConsoleInterface
{
    private: class _OCLPlatformsCommand : public ConsoleCommand
    {
        public: _OCLPlatformsCommand(Console &console) :
            ConsoleCommand(
                "OCLPlatforms",
                "OCLPlatforms\n"
                "Prints avaliable OpenCL platforms info.\n"
                "Takes no arguments.\n",
                console){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override
        {
            if(argv.size() != 0)
            {
                getConsole().getOutputStream() << "Error: wrong number of arguments.\n";
                return -1;
            }
            getConsole().getOutputStream() << OpenCL::CLManager::instance().printPlatformsInfo();
            return 0;
        }
    } *_commandOCLPlatforms = nullptr;

    private: class _OCLDevicesCommand : public ConsoleCommand
    {
        public: _OCLDevicesCommand(Console &console) :
            ConsoleCommand(
                "OCLDevices",
                "OCLDevices\n"
                "Prints avaliable OpenCL devives info.\n"
                "Takes no arguments.\n",
                console){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override
        {
            if(argv.size() != 0)
            {
                getConsole().getOutputStream() << "Error: wrong number of arguments.\n";
                return -1;
            }
            getConsole().getOutputStream() << OpenCL::CLManager::instance().printDevicesInfo();
            return 0;
        }
    } *_commandOCLDevices = nullptr;

    public : CLManagerConsoleInterface(Console &console):
        _commandOCLPlatforms(new _OCLPlatformsCommand(console)),
        _commandOCLDevices(new _OCLDevicesCommand(console))
        {}

    public : ~CLManagerConsoleInterface()
    {
        delete _commandOCLPlatforms;
        delete _commandOCLDevices;
    }
};
}

#endif // CLMANAGERCONSOLEINTERFACE

