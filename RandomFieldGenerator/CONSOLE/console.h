#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>

#include "CONSOLE/consolecommand.h"

namespace Controller
{
class Console
{
    protected: std::ostream &_outputStream;
    /// \warning this usage don't does the logging
    public : std::ostream &getOutputStream() noexcept {return _outputStream;}
    protected: std::istream &_inputStream;
//    public : std::istream &getInputStream() noexcept {return _inputStream;}
    private: std::map<std::string, ConsoleCommand*> _commands;
    private: volatile bool _isRuning = false;

    /// bad, when wrong command is given or when executeConsoleCommand() returns != 0
    private: bool _badLastCommand = false;
    public : bool isLastCommandBad() const noexcept {return _badLastCommand;}
    /// for manual usage
    public : void setLastCommandBadState(bool isBad) noexcept {_badLastCommand = isBad;}

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
             << "Type 'help' or '\\?' and 'command name' to see command help message."
             << std::endl
             << "Type 'exit' to exit the console." << std::endl;
        return _str.str();
    }

    public : virtual void writeToOutput(const std::string &str) noexcept
    {
        _outputStream << str;
    }

    public : virtual void echoInput(const std::string &str) noexcept
    {
        // should be clean
    }

    public : void operator << (const std::string &str) noexcept
    {
        echoInput(str);

        _badLastCommand = false;
        std::stringstream _output;

        std::istringstream _strStreamWrap{str};
        std::string _commandName;
        _strStreamWrap >> _commandName; // Get firs word
        std::map<std::string, ConsoleCommand*>::iterator _curCommand =
                _commands.find(_commandName);
        if(_curCommand != _commands.end())
        {
            // construct arguments
            std::vector<std::string> _args;
            while(_strStreamWrap >> _commandName)
                _args.push_back(_commandName);
            if(_curCommand->second->executeConsoleCommand(_args))
                _badLastCommand = true;
        }
        else if(_commandName.compare("help") == 0 || _commandName.compare("\\?") == 0)
        {
            if(_strStreamWrap >> _commandName) // Get second word
            {
                _curCommand = _commands.find(_commandName);
                if(_curCommand != _commands.end())
                {
                    _output << _curCommand->second->getCommandHelp();
                }
                else
                {
                    _output << _wrongCommandMsg(_commandName);
                    _badLastCommand = true;
                }
            }
            else
            {
                _output << _getConsoleHelpMsg();
            }
        }
        else if(_commandName.compare("exit") == 0)
        {
            _isRuning = false;
        }
        else
        {
            _output << _wrongCommandMsg(_commandName);
            _badLastCommand = true;
        }
        _outputStream << ">";

        if(!_output.str().empty())
            writeToOutput(_output.str());
    }

    public: void runMainLoop()
    {
//        _outputStream << _getConsoleHelpMsg() << '>';
        _isRuning = true;
        char _c[255];
        for(;_isRuning;)
        {
            _inputStream.getline(_c, 256);
            std::string _str(_c);
            (*this) << _str;
        }
    }

    /// executeScript --------------------------------------------------------------------------
    public : std::string executeScript(const std::string &fileName) noexcept
    {
        std::ifstream _scriptFileStream;
        try
        {
            _scriptFileStream.exceptions(std::ifstream::badbit);
            _scriptFileStream.open(fileName, std::ios::in);
            if(_scriptFileStream.is_open())
            {
                _scriptFileStream.seekg(0, std::ios::beg);
                std::string _scriptLine;
                while(getline(_scriptFileStream, _scriptLine))
                {
                    _outputStream << _scriptLine << "\n";
                    (*this) << _scriptLine;
                    if(_badLastCommand)
                        throw(std::runtime_error("Fail to execute script"));
                }
                _scriptFileStream.close();
            }
            return "Script from " + fileName + " executed.\n";
        }
        catch(std::exception &e)
        {
            if(_scriptFileStream.is_open())
                _scriptFileStream.close();
            std::stringstream _str;
            _str << "Error: " << e.what() << "\n";
            if(_scriptFileStream.fail() || _scriptFileStream.eof() || _scriptFileStream.bad())
            _str << "  failbit: " << _scriptFileStream.fail() <<"\n"
                 << "  eofbit: " << _scriptFileStream.eof() <<"\n"
                 << "  badbit: " << _scriptFileStream.bad() <<"\n";
            return _str.str();
        }
    }
    private: class _executeScriptCommand : public ConsoleCommand
    {
        public: _executeScriptCommand(Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "executeScript",
                "executeScript <fileName>\n"
                "Execute a command script saved in <filename>.\n"
                "Arguments:\n"
                "[string] <fileName> - the name of the file (recommended extension *.scr).\n",
                console){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override
        {
            if(argv.size() != 1)
            {
                getConsole().writeToOutput("Error: wrong number of arguments.\n");
                return -1;
            }
            getConsole().writeToOutput(getConsole().executeScript(argv[0]));
            return 0;
        }
    } *_executeScript = nullptr;

    public : Console(std::ostream &outputStream, std::istream &inputStream) noexcept :
        _outputStream(outputStream),
        _inputStream(inputStream),
        _executeScript(new _executeScriptCommand(*this))
    {}

    public : ~Console()
    {
        delete _executeScript;
    }
};
}

#endif // CONSOLE_H
