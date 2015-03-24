#ifndef CLMANAGERCONSOLEINTERFACE
#define CLMANAGERCONSOLEINTERFACE

#include "console.h"
#include "CLMANAGER/clmanager.h"
#include "consolecommand.h"
#include "UI/clmanagergui.h"
#include "UI/userinterfacemanager.h"

namespace Controller
{

/// \warning don't make direct usage! it should be nested, but in that case causes Qt error
class _OCLSetupGUICommand : public QObject, public ConsoleCommand
{
    Q_OBJECT

    public: _OCLSetupGUICommand(Console &console) :
        ConsoleCommand(
            "OCLSetupGUI",
            "OCLSetupGUI\n"
            "Call the Graphical User Interface OpenCl setup form.\n"
            "Takes no arguments.\n",
            console)
    {
        connect(this, SIGNAL(signal_OCLSetupGUIStart()),
                &UserInterface::UserInterfaceManager::instance(), SLOT(OCLSetupGUIStart()));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_OCLSetupGUIError()),
                this, SLOT(OCLSetupGUIError()));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_OCLSetupGUIFinish()),
                this, SLOT(OCLSetupGUIFinish()));
    }
    public: Q_SIGNAL void signal_OCLSetupGUIStart();
    public: Q_SLOT void OCLSetupGUIFinish(){
        getConsole().getOutputStream() << "OpenCL Setup GUI finish.\n";}
    public: Q_SLOT void OCLSetupGUIError(){
        getConsole().getOutputStream() << "OpenCL Setup GUI is already running.\n";}

    public: int executeConsoleCommand(const std::vector<std::string> &argv) override
    {
        if(argv.size() != 0)
        {
            getConsole().getOutputStream() << "Error: wrong number of arguments.\n";
            return -1;
        }
        getConsole().getOutputStream() << "OpenCL Setup GUI start.\n";
        Q_EMIT signal_OCLSetupGUIStart();
        return 0;
    }
};

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

    private: _OCLSetupGUICommand *_commandOCLSetupGUI = nullptr;

    public : CLManagerConsoleInterface(Console &console):
        _commandOCLPlatforms(new _OCLPlatformsCommand(console)),
        _commandOCLDevices(new _OCLDevicesCommand(console)),
        _commandOCLSetupGUI(new _OCLSetupGUICommand(console))
        {}

    public : ~CLManagerConsoleInterface()
    {
        delete _commandOCLPlatforms;
        delete _commandOCLDevices;
        delete _commandOCLSetupGUI;
    }
};
}

#endif // CLMANAGERCONSOLEINTERFACE

