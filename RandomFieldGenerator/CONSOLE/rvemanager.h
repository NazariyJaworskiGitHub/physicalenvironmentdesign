#ifndef RVEMANAGER
#define RVEMANAGER

#include <map>

#include "representativevolumeelement.h"
#include "consolecommand.h"

class RVEManager
{
    private: std::map<std::string, RepresentativeVolumeElement*> _RVEs;
    public : std::string createRVE(const std::string &name, int size) noexcept
    {
        try
        {
            if(_RVEs.find(name) != _RVEs.end())
                return "Error: Representative Volume Element " + name + " already exists.\n";
            else if((size >= 2) && ((size & (size - 1)) == 0)) // check power o two
            {
                _RVEs.emplace(name, new RepresentativeVolumeElement(size));
            }
            else return "Error: Cant create Representative Volume Element with given size.\n";
        }
        catch(std::exception &e)
        {
            return e.what();
        }
        return "Representative Volume Element " + name + " created.\n";
    }
    private: class _CreateRVECommand : public ConsoleCommand
    {
        private: RVEManager &_manager;
        public: _CreateRVECommand(RVEManager &manager, Console &console) :
            ConsoleCommand(
                "createRVE",
                "createRVE <Name> <size>\n"
                "Creates Representative Volume Element (RVE) object in RAM memory.\n"
                "Arguments:\n"
                " <Name> - the name of RVE in RAM memory,\n"
                " <size> - the discrete size of RVE. It should be equal to some power of two.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override
        {
            if(argv.size() != 2)
            {
                getConsole().getOutputStream() << "Error: wrong number of arguments.\n";
                return -1;
            }
            getConsole().getOutputStream() << _manager.createRVE(argv[0], std::atoi(argv[1].data()));
            return 0;
        }
    } *_commandCreateRVE = nullptr;

    public : std::string deleteRVE(const std::string &name) noexcept
    {
        try
        {
            auto _pos = _RVEs.find(name);
            if(_pos == _RVEs.end())
                return "Error: Representative Volume Element " + name + " doesn't exist.\n";
            else
                _RVEs.erase(_pos);
        }
        catch(std::exception &e)
        {
            return e.what();
        }
        return "Representative Volume Element " + name + " deleted.\n";
    }
    private: class _DeleteRVECommand : public ConsoleCommand
    {
        private: RVEManager &_manager;
        public: _DeleteRVECommand(RVEManager &manager, Console &console) :
            ConsoleCommand(
                "deleteRVE",
                "deleteRVE <Name>\n"
                "Deletes Representative Volume Element (RVE) object from RAM memory.\n"
                "Arguments:\n"
                " <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override
        {
            if(argv.size() != 1)
            {
                getConsole().getOutputStream() << "Error: wrong number of arguments.\n";
                return -1;
            }
            getConsole().getOutputStream() << _manager.deleteRVE(argv[0]);
            return 0;
        }
    } *_commandDeleteRVE = nullptr;

    public : std::string printRVE() noexcept
    {
        if(_RVEs.empty())
            return "There are no Representative Volume Element (RVE) objects in memory.\n";

        std::stringstream _str;
        int _i = 0;
        for(auto _rve: _RVEs)
            _str << " [" << _i++ << "]: " << _rve.first << " "
                 <<  _rve.second->getSize() << std::endl;
        return _str.str();
    }
    private: class _PrintRVECommand : public ConsoleCommand
    {
        private: RVEManager &_manager;
        public: _PrintRVECommand(RVEManager &manager, Console &console) :
            ConsoleCommand(
                "printRVE",
                "createRVE\n"
                "Prints existing in RAM memory Representative Volume Element (RVE) objects in\n"
                "format: [<index>]: <name> <size>.\n"
                "Takes no arguments.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override
        {
            if(argv.size() != 0)
            {
                getConsole().getOutputStream() << "Error: wrong number of arguments.\n";
                return -1;
            }
            getConsole().getOutputStream() << _manager.printRVE();
            return 0;
        }
    } *_commandPrintRVE = nullptr;

    public : RVEManager(Console &console):
        _commandCreateRVE(new _CreateRVECommand(*this, console)),
        _commandDeleteRVE(new _DeleteRVECommand(*this, console)),
        _commandPrintRVE(new _PrintRVECommand(*this, console))
        {}
    public : ~RVEManager()
    {
        for(auto _rve: _RVEs)
            delete _rve.second;
        delete _commandCreateRVE;
        delete _commandDeleteRVE;
        delete _commandPrintRVE;
    }
};

#endif // RVEMANAGER
