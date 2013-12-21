/// \file \author Nazariy Jaworski

#include "dummy_wserver.h"

// WARNING! It does dependence for all usage of WServer

using namespace Wt;

/// Dummy flag
static bool _serverRunningFlag = false;
/// Dummy stub
bool WServer::start()
{
    return _serverRunningFlag = true;
}
/// Dummy stub
void WServer::stop()
{
    _serverRunningFlag = false;
}
/// Dummy stub
int WServer::httpPort() const
{
    return 0;
}
/// Dummy stub
bool WServer::isRunning() const
{
    return _serverRunningFlag;
}
/// Dummy stub
void WServer::setServerConfiguration(
        int argc, char *argv[], const std::string &serverConfigurationFile)
{
    for(int i=0; i<argc; i++)
        if(!argv[i] || !argv[i][0])
            throw std::runtime_error("Dummy server ERROR: bad parameters\n");
}
