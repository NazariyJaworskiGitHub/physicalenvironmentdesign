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
        _RVEName = argv[0];
        getConsole().writeToOutput("Edit RVE GUI start.\n");
        Q_EMIT signal_editRVEGUIStart(_pos->second);
        return 0;
    }
}

/// \todo send true command
void _EditRVECommand::applyGaussFltrRVE(
        int discreteRadius,
        float ellipsoidScaleFactorX,
        float ellipsoidScaleFactorY,
        float ellipsoidScaleFactorZ)
{
    {
        std::stringstream _str;
        _str << "cleanRVE " << _RVEName << "\n";
        getConsole().writeToOutput(_str.str());
    }
    getConsole().writeToOutput(_manager.cleanRVE(_RVEName));

    {
        std::stringstream _str;
        _str << "addRandomNoiseRVE " << _RVEName << "\n";
        getConsole().writeToOutput(_str.str());
    }
    getConsole().writeToOutput(_manager.addRandomNoiseRVE(_RVEName));

    {
        std::stringstream _str;
        _str << "applyGaussianFilterRVE " << _RVEName << " "
             << discreteRadius << " "
             << ellipsoidScaleFactorX << " "
             << ellipsoidScaleFactorY << " "
             << ellipsoidScaleFactorZ << "\n";
        getConsole().writeToOutput(_str.str());
    }
    /// \todo new arguments
    getConsole().writeToOutput(_manager.applyGaussianFilterRVE(
                                   _RVEName,
                                   discreteRadius,
                                   ellipsoidScaleFactorX,
                                   ellipsoidScaleFactorY,
                                   ellipsoidScaleFactorZ,
                                   false,
                                   1.0f));

    Q_EMIT signal_applyGaussFltrRVEDone();
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

std::string RepresentativeVolumeElementConsoleInterface::cleanRVE(
        const std::string &name) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->cleanData();
    return "Representative Volume Element " + name + " cleaned.\n";
}

int RepresentativeVolumeElementConsoleInterface::_cleanRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 1)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.cleanRVE(argv[0]));
    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::cleanUnMaskedRVE(
        const std::string &name, float filler) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->cleanUnMaskedData(filler);
    return "Representative Volume Element " + name + " cleaned.\n";
}

int RepresentativeVolumeElementConsoleInterface::_cleanUnMaskedRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() < 1 || argv.size() > 2)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    else if(argv.size() == 1)
        getConsole().writeToOutput(_manager.cleanUnMaskedRVE(argv[0], 0.0f));
    else
        getConsole().writeToOutput(_manager.cleanUnMaskedRVE(argv[0], std::atof(argv[1].data())));
    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::cleanMaskRVE(
        const std::string &name) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->cleanMask();
    return "Representative Volume Element mask" + name + " cleaned.\n";
}

int RepresentativeVolumeElementConsoleInterface::_cleanMaskRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 1)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.cleanMaskRVE(argv[0]));
    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::addRandomNoiseRVE(
        const std::string &name) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->addRandomNoise();
    return "Representative Volume Element " + name + " random noise generation done.\n";
}

int RepresentativeVolumeElementConsoleInterface::_addRandomNoiseRVECommandmmand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 1)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.addRandomNoiseRVE(argv[0]));
    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::normalizeUnMaskedRVE(
        const std::string &name) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->normalizeUnMasked();
    return "Representative Volume Element " + name + " normalized.\n";
}

int RepresentativeVolumeElementConsoleInterface::_normalizeUnMaskedRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 1)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.normalizeUnMaskedRVE(argv[0]));
    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::invertUnMaskedRVE(
        const std::string &name) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->invertUnMasked();
    return "Representative Volume Element " + name + " inverted.\n";
}

int RepresentativeVolumeElementConsoleInterface::_invertUnMaskedRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 1)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.invertUnMaskedRVE(argv[0]));
    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::applyGaussianFilterRVE(
        const std::string &name,
        int discreteRadius,
        float ellipsoidScaleFactorX,
        float ellipsoidScaleFactorY,
        float ellipsoidScaleFactorZ,
        bool useDataAsIntensity,
        float intensityFactor)
{
    try
    {
        auto _pos = RVEs.find(name);
        if(_pos == RVEs.end())
            return "Error: Representative Volume Element " + name + " doesn't exist.\n";
        else
            _pos->second->applyGaussianFilterCL(
                        discreteRadius,
                        ellipsoidScaleFactorX,
                        ellipsoidScaleFactorY,
                        ellipsoidScaleFactorZ,
                        useDataAsIntensity,
                        intensityFactor);
    }
    catch(std::exception &e)
    {
        return e.what();
    }
    return "Representative Volume Element " + name + " Gaussian blur filter applying done.\n";
}

int RepresentativeVolumeElementConsoleInterface::_applyGaussianFilterRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() < 2 || argv.size() > 7)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    int discreteRadius = std::atoi(argv[1].data());
    float ellipsoidScaleFactorX = 1.0f;
    float ellipsoidScaleFactorY = 1.0f;
    float ellipsoidScaleFactorZ = 1.0f;
    bool useDataAsIntensity = false;
    float intensityFactor = 1.0f;
    switch(argv.size())
    {
    case 7: intensityFactor =std::atof(argv[6].data());
    case 6: if(argv[5].compare("true") == 0) useDataAsIntensity = true;
        else if(argv[5].compare("false") == 0) useDataAsIntensity = false;
        else
        {
            getConsole().writeToOutput("wrong <useDataAsIntensity> argument\n");
            return -1;
        }
    case 5: ellipsoidScaleFactorZ = std::atof(argv[4].data());
    case 4: ellipsoidScaleFactorY = std::atof(argv[3].data());
    case 3: ellipsoidScaleFactorX = std::atof(argv[2].data());
    }

    getConsole().writeToOutput(_manager.applyGaussianFilterRVE(
            argv[0],
            discreteRadius,
            ellipsoidScaleFactorX,
            ellipsoidScaleFactorY,
            ellipsoidScaleFactorZ,
            useDataAsIntensity,
            intensityFactor));

    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::applyTwoCutMaskInsideRVE(
        const std::string &name, float cutLevelA, float cutLevelB) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->applyTwoCutMaskInside(cutLevelA, cutLevelB);
    return "Representative Volume Element mask" + name + " is set.\n";
}

int RepresentativeVolumeElementConsoleInterface::_applyTwoCutMaskInsideRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 3)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.applyTwoCutMaskInsideRVE(
                                   argv[0], std::atof(argv[1].data()), std::atof(argv[2].data())));
    return 0;
}

std::string RepresentativeVolumeElementConsoleInterface::applyTwoCutMaskOutsideRVE(
        const std::string &name, float cutLevelA, float cutLevelB) noexcept
{
    auto _pos = RVEs.find(name);
    if(_pos == RVEs.end())
        return "Error: Representative Volume Element " + name + " doesn't exist.\n";
    else
        _pos->second->applyTwoCutMaskOutside(cutLevelA, cutLevelB);
    return "Representative Volume Element mask" + name + " is set.\n";
}

int RepresentativeVolumeElementConsoleInterface::_applyTwoCutMaskOutsideRVECommand::executeConsoleCommand(
        const std::vector<std::string> &argv)
{
    if(argv.size() != 3)
    {
        getConsole().writeToOutput("Error: wrong number of arguments.\n");
        return -1;
    }
    getConsole().writeToOutput(_manager.applyTwoCutMaskOutsideRVE(
                                   argv[0], std::atof(argv[1].data()), std::atof(argv[2].data())));
    return 0;
}
