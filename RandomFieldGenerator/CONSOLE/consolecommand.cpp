#include "consolecommand.h"
#include "console.h"

ConsoleCommand::ConsoleCommand(
        const std::string commandName,
        const std::string commandHelp,
        Console &console) noexcept :
    _name(commandName), _help(commandHelp), _console(console)
{
    _console.addCommand(this);
}
