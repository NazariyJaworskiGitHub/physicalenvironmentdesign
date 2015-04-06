#include "representativevolumeelementconsoleinterface.h"

using namespace Controller;

int _EditRVECommand::executeConsoleCommand(const std::vector<std::string> &argv)
{
    if(argv.size() != 1)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    auto _pos = _manager.RVEs.find(argv[0]);
    if(_pos == _manager.RVEs.end())
    {
        getConsole().writeToOutput("Error: Representative Volume Element " +
                                   argv[0] + " doesn't exist.\n");
        return -1;
    }
    else
    {
        getConsole().writeToOutput("Edit RVE GUI start.\n");
        Q_EMIT signal_editRVEGUIStart(_pos->second);
        return 0;
    }
}


std::string RepresentativeVolumeElementConsoleInterface::createRVE(
        const std::string &name, int size) noexcept
{
    try
    {
        if(RVEs.find(name) != RVEs.end())
            return "Error: Representative Volume Element " + name + " already exists.\n";
        else if((size >= 2) && ((size & (size - 1)) == 0)) // check power o two
        {
            RVEs.emplace(name, new RepresentativeVolumeElement(size));
        }
        else return "Error: Cant create Representative Volume Element with given size.\n";
    }
    catch(std::exception &e)
    {
        return e.what();
    }
    return "Representative Volume Element " + name + " created.\n";
}


int RepresentativeVolumeElementConsoleInterface::_CreateRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 2)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.createRVE(argv[0], std::atoi(argv[1].data())));
    return 0;
}


std::string RepresentativeVolumeElementConsoleInterface::deleteRVE(
        const std::string &name) noexcept
{
    try
    {
        auto _pos = RVEs.find(name);
        if(_pos == RVEs.end())
            return "Error: Representative Volume Element " + name + " doesn't exist.\n";
        else
            RVEs.erase(_pos);
    }
    catch(std::exception &e)
    {
        return e.what();
    }
    return "Representative Volume Element " + name + " deleted.\n";
}


int RepresentativeVolumeElementConsoleInterface::_DeleteRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 1)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.deleteRVE(argv[0]));
    return 0;
}


std::string Controller::RepresentativeVolumeElementConsoleInterface::printRVE() noexcept
{
    if(RVEs.empty())
        return "There are no Representative Volume Element (RVE) objects in memory.\n";

    std::stringstream _str;
    int _i = 0;
    for(auto _rve: RVEs)
        _str << " [" << _i++ << "]: " << _rve.first << " "
             <<  _rve.second->getSize() << std::endl;
    return _str.str();
}


int RepresentativeVolumeElementConsoleInterface::_PrintRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 0)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.printRVE());
    return 0;
}
