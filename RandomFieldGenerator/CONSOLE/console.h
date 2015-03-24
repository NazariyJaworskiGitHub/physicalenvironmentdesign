#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

#include "CONSOLE/consolecommand.h"

namespace Controller
{
class Console
{
    private: std::ostream &_outputStream;
    public : std::ostream &getOutputStream() noexcept {return _outputStream;}
    private: std::istream &_inputStream;
    public : std::istream &getInputStream() noexcept {return _inputStream;}
    private: std::map<std::string, ConsoleCommand*> _commands;
    private: volatile bool _isRuning = false;

    public : void addCommand(ConsoleCommand *command) noexcept
    {
        _commands.emplace(command->getCommandName(), command);
    }

    private: std::string _wrongCommandMsg(const std::string &name) const noexcept
    {
        std::stringstream _str;
        _str << "'" << name << "' is not recognized as an internal command." << std::endl;
        return _str.str();
    }

    private: std::string _getConsoleHelpMsg() const noexcept
    {
        std::stringstream _str;
        _str << "Console v0.1" << std::endl
             << "Commands:" << std::endl;
        for(auto i : _commands)
            _str << " " << i.first << std::endl;
        _str << "Type 'help' or '\\?' to see this message again." << std::endl
             << "Type 'help' or '\\?' and 'command name' to see command help message." << std::endl
             << "Type 'exit' to exit the console." << std::endl;
        return _str.str();
    }

    public : Console(std::ostream &outputStream, std::istream &inputStream) noexcept :
        _outputStream(outputStream), _inputStream(inputStream){}

    public : ~Console(){}

    public : void operator << (const std::string str) noexcept
    {
        std::istringstream _strStreamWrap{str};
        std::string _commandName;
        _strStreamWrap >> _commandName; // Get firs word
        std::map<std::string, ConsoleCommand*>::iterator _curCommand = _commands.find(_commandName);
        if(_curCommand != _commands.end())
        {
            // construct arguments
            std::vector<std::string> _args;
            while(_strStreamWrap >> _commandName)
                _args.push_back(_commandName);
            _curCommand->second->executeConsoleCommand(_args);
        }
        else if(_commandName.compare("help") == 0 || _commandName.compare("\\?") == 0)
        {
            if(_strStreamWrap >> _commandName) // Get second word
            {
                _curCommand = _commands.find(_commandName);
                if(_curCommand != _commands.end())
                {
                    _outputStream << _curCommand->second->getCommandHelp();
                }
                else
                    _outputStream << _wrongCommandMsg(_commandName);
            }
            else
            {
                _outputStream << _getConsoleHelpMsg();
            }
        }
        else if(_commandName.compare("exit") == 0)
        {
            _isRuning = false;
        }
        else
        {
            _outputStream << _wrongCommandMsg(_commandName);
        }
        _outputStream << '>';
    }

    public: void runMainLoop()
    {
        _outputStream << _getConsoleHelpMsg() << '>';
        _isRuning = true;
        char _c[255];
        for(;_isRuning;)
        {
            _inputStream.getline(_c, 256);
            std::string _str(_c);
            (*this) << _str;
        }
    }
};
}

#endif // CONSOLE_H
