#ifndef CONSOLECOMMAND_H
#define CONSOLECOMMAND_H

#include <string>
#include <vector>

class Console;

class ConsoleCommand
{
    private: const std::string _name;
    public : const std::string &getCommandName() const noexcept {return _name;}
    private: const std::string _help;
    public : const std::string &getCommandHelp() const noexcept {return _help;}
    private: Console &_console;
    public : Console &getConsole() noexcept {return _console;}

    /// New command will be added to specific console on construction
    public : ConsoleCommand(
            const std::string commandName,
            const std::string commandHelp,
            Console &console) noexcept;

    /// Override this
    public : virtual int executeConsoleCommand(const std::vector<std::string> &argv) = 0;

    public : ~ConsoleCommand(){}
};

#endif // CONSOLECOMMAND_H
