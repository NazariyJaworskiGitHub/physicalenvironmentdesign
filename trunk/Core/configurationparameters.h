/// \file \author Nazariy Jaworski

#ifndef CONFIGURATIONPARAMETERS_H
#define CONFIGURATIONPARAMETERS_H

#define CONFIGURATION_FILE_KEYWORD_TCPPORT                  "TCPPORT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERDOCROOT       "UIWEBSERVERDOCROOT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPADDRESS   "UIWEBSERVERHTTPADDRESS"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPPORT      "UIWEBSERVERHTTPPORT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERAPPROOT       "UIWEBSERVERAPPROOT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERCONFIG        "UIWEBSERVERCONFIG"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERACCESSLOG     "UIWEBSERVERACCESSLOG"

#include <QtGlobal>

/// Configuration parameters structure, see Core::_readConfigurationFile()
/// \todo put here more parameters
/// \todo make class
struct ConfigurationParameters
{
    //qint16  tcpServerPort;

    //--docroot .
    //--http-address 0.0.0.0
    //--http-port 81
    //--approot .
    //--config wtconfig.xml
    //--accesslog AccesLog.log
    /// Configuration parameters for UiWebServer
    struct WebServerParameters
    {
        char*   uiWebServerDocRoot      = nullptr;
        char*   uiWebServerHttpAddress  = nullptr;
        char*   uiWebServerHttpPort     = nullptr;
        char*   uiWebServerAppRoot      = nullptr;
        char*   uiWebServerConfig       = nullptr;
        char*   uiWebServerAccessLog    = nullptr;
    } webServerParameters;
};

#endif // CONFIGURATIONPARAMETERS_H
