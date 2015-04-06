#ifndef RVEMANAGER
#define RVEMANAGER

#include <map>

#include "UI/userinterfacemanager.h"    // Include it first to fix OpenGL/GLEW compatibility
#include "console.h"
#include "representativevolumeelement.h"
#include "consolecommand.h"


namespace Controller
{

class RepresentativeVolumeElementConsoleInterface;

/// editRVE --------------------------------------------------------------------------------
/// \warning don't make direct usage! it should be nested, but in that case causes Qt error
class _EditRVECommand : public QObject, public ConsoleCommand
{
    Q_OBJECT

    private: RepresentativeVolumeElementConsoleInterface &_manager;
    public: _EditRVECommand(
            RepresentativeVolumeElementConsoleInterface &manager,
            Console &console) :
        ConsoleCommand(
            "editRVE",
            "editRVE\n"
            "Call the Graphical User Interface Representative Volume Element edit form.\n"
            "Arguments:\n"
            " <Name> - the name of RVE in RAM memory.\n",
            console),
        _manager(manager)
    {
        connect(this, SIGNAL(signal_editRVEGUIStart(RepresentativeVolumeElement*)),
                &UserInterface::UserInterfaceManager::instance(),
                SLOT(editRVEGUIStart(RepresentativeVolumeElement*)));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_editRVEGUIError()),
                this, SLOT(editRVEGUIError()));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_editRVEGUIFinish()),
                this, SLOT(editRVEGUIFinish()));
    }
    public: Q_SIGNAL void signal_editRVEGUIStart(RepresentativeVolumeElement* ptrToRVE);
    public: Q_SLOT void editRVEGUIFinish(){
        getConsole().writeToOutput("Edit RVE GUI finish.\n");}
    public: Q_SLOT void editRVEGUIError(){
        getConsole().writeToOutput("Edit RVE GUI is already running.\n");}

    public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
};

class RepresentativeVolumeElementConsoleInterface
{
    public : std::map<std::string, RepresentativeVolumeElement*> RVEs;

    /// createRVE ----------------------------------------------------------------------------
    public : std::string createRVE(const std::string &name, int size) noexcept;
    private: class _CreateRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _CreateRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
                "createRVE",
                "createRVE <Name> <size>\n"
                "Creates Representative Volume Element (RVE) object in RAM memory.\n"
                "Arguments:\n"
                " <Name> - the name of RVE in RAM memory,\n"
                " <size> - the discrete size of RVE. It should be equal to some power of two.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandCreateRVE = nullptr;

    /// deleteRVE ----------------------------------------------------------------------------
    public : std::string deleteRVE(const std::string &name) noexcept;
    private: class _DeleteRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _DeleteRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
                "deleteRVE",
                "deleteRVE <Name>\n"
                "Deletes Representative Volume Element (RVE) object from RAM memory.\n"
                "Arguments:\n"
                " <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandDeleteRVE = nullptr;

    /// printRVE -----------------------------------------------------------------------------
    public : std::string printRVE() noexcept;
    private: class _PrintRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _PrintRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
                "printRVE",
                "createRVE\n"
                "Prints existing in RAM memory Representative Volume Element (RVE) objects in\n"
                "format: [<index>]: <name> <size>.\n"
                "Takes no arguments.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandPrintRVE = nullptr;

    _EditRVECommand *_commandEditRVE = nullptr;

    public : RepresentativeVolumeElementConsoleInterface(Console &console):
        _commandCreateRVE(new _CreateRVECommand(*this, console)),
        _commandDeleteRVE(new _DeleteRVECommand(*this, console)),
        _commandPrintRVE(new _PrintRVECommand(*this, console)),
        _commandEditRVE(new _EditRVECommand(*this, console))
        {}
    public : ~RepresentativeVolumeElementConsoleInterface()
    {
        for(auto _rve: RVEs)
            delete _rve.second;
        delete _commandCreateRVE;
        delete _commandDeleteRVE;
        delete _commandPrintRVE;
        delete _commandEditRVE;
    }
};
}

#endif // RVEMANAGER
