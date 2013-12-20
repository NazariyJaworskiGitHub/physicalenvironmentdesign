/// \file \author Nazariy Jaworski

#ifndef DUMMY_WSERVER_H
#define DUMMY_WSERVER_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <Wt/WServer>

// Implementation of dummy Test-case WServer (conflict between libwthttp & libwttest)
// WARNING! It does dependence for all usage of WServer
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
}

#endif // DUMMY_WSERVER_H
