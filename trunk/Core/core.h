/// \author Nazariy Jaworski

#ifndef CORE_H
#define CORE_H

#include <QCoreApplication>
#include <QTcpServer>

#include "guard.h"
#include "logger.h"
#include "Session/usersession.h"
#include "Ui/Web/uiwebserver.h"

/// \todo move this out of here
#define CONFIGURATION_FILE_KEYWORD_TCPPORT                  "TCPPORT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERDOCROOT       "UIWEBSERVERDOCROOT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPADDRESS   "UIWEBSERVERHTTPADDRESS"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPPORT      "UIWEBSERVERHTTPPORT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERAPPROOT       "UIWEBSERVERAPPROOT"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERCONFIG        "UIWEBSERVERCONFIG"
#define CONFIGURATION_FILE_KEYWORD_UIWEBSERVERACCESSLOG     "UIWEBSERVERACCESSLOG"

/// Main program class, does all dirty work. \n
/// There is only one instance of this class object per program.
/// Inherits main event loop.
/// \todo make all qt-connect() outside from constructors
// object has CommandServer object, which is processing commands
// from users through TCP port. \n
class Core : public QCoreApplication
{
    Q_OBJECT

    /// \todo move this out of here
    private : struct ConfigurationParameters
    {
        qint16  tcpServerPort;
        char*   uiWebServerDocRoot      = nullptr;
        char*   uiWebServerHttpAddress  = nullptr;
        char*   uiWebServerHttpPort     = nullptr;
        char*   uiWebServerAppRoot      = nullptr;
        char*   uiWebServerConfig       = nullptr;
        char*   uiWebServerAccessLog    = nullptr;
    };
    /// Configuration parameters,
    /// use readConfigurationFile() to fill them
    public : ConfigurationParameters myConfigurationParameters;

    // Local objects
        /// Should be set to true after succeseful call of Init()
    private: bool _isInitialized;
        /// Returns the value of _isInitialized
    public : bool isInitialized() const {return _isInitialized;}

        /// For TCP communications
    //private: CommandServer *_myQTcpServer = nullptr;

        /// Makes users authentication
    public : Guard *myGuard = nullptr;
        /// Provides Web-Ui
    public : Ui::Web::UiWebServer *myUiWebServer = nullptr;

        /// Logger for this object
    public : Logger *myLogger = nullptr;

        /// Holds pointers to UserSession,
        /// each UserSession should be created at UiWebServer::createApplication(),
        /// or at something like that
        /// not used for now
        /// \todo fill the list!
    public : QList<Session::UserSession*> myUserSessions;

    // Main functions
        /// Common constructor, don't forget to call init()
    private: Core(int argc, char **argv);
        /// Initialization, if done, makes _isInitialized = true
    public : Q_SLOT void init();
        /// Common destructor
    private: ~Core();
        /// Getting the Instance of Core singleton
    public : static Core *instance();

    // Configurations
        /// Reads configuration from \c config.cfg, fills myConfigurationParameters
    private: Q_SLOT void readConfigurationFile();

    // Network
    //    /// Starts or restarts _myQTcpServer
    //private: Q_SLOT void runTcpServer();
        /// Starts or restarts myUiWebServer
    private: Q_SLOT void runUiWebServer();

    // Errors
        /// Handler for exceptions, write error-message to log and exit from the program.
        /// emits writeString(), creates \c LastCrashReport.log with message
    public : Q_SLOT void fatalError(const QString message);

    // Messaging
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
};

#endif // CORE_H
