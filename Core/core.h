/// \file \author Nazariy Jaworski

#ifndef CORE_H
#define CORE_H

#include "exception"

#include <QCoreApplication>
#include <QTcpServer>

#include "configurationparameters.h"
#include "guard.h"
#include "logger.h"
#include "Session/usersession.h"
#include "Ui/Web/uiwebserver.h"

/// Main program class, does all dirty work. \n
/// Inherits main event loop.
/// There is only one instance of this class object per program.
/// This instance is the environment for all server-objects (global-agents),
/// use them by Core::instance(), create them by Core::initialize().
/// \n throws std::exception
/// \todo make all qt-connect() outside from constructors
/// \todo make exceptions catch
// object has CommandServer object, which is processing commands
// from users through TCP port. \n
class Core : public QCoreApplication
{
    Q_OBJECT

        /// Configuration parameters,
        /// use _readConfigurationFile() to fill them
    public : ConfigurationParameters myConfigurationParameters;

    // Local objects
        /// Should be set to true after succeseful call of initialize()
    private: bool _isInitialized;
        /// Returns the value of _isInitialized
    public : bool isInitialized() const {return _isInitialized;}

    /*    /// For TCP communications
    private: CommandServer *_myQTcpServer = nullptr;*/

        /// Makes users authentication
    public : Guard *myGuard = nullptr;
        /// Provides Web-Ui
    public : Ui::Web::UiWebServer *myUiWebServer = nullptr;

        /// Logger for all objects, use writeString() signal to log down the message
    public : Logger *myLogger = nullptr;

    /*    /// Holds pointers to UserSession,
        /// each UserSession should be created at UiWebServer::createApplication(),
        /// or at something like that
        /// not used for now
        /// \todo fill the list!
    public : QList<Session::UserSession*> myUserSessions;*/

    // Main functions
        /// Common constructor, don't forget to call initialize()
    private: Core();
        /// Initialization, if done, makes _isInitialized = true
        /// Steps:
        /// \li check _isInitialized if false, go next, else return;
        /// \li _readConfigurationFile();
        /// \li create myLogger;
        /// \li open logging file;
        /// \li create myGuard;
        /// \li read usersFile for myGuard;
        /// \li create myUiWebServer;
        /// \li start myUiWebServer;
        /// \li set _isInitialized to true;
        /// \li emit info-message by writeString();
        /// \li if some error have a place, emit fatalError();
    public : Q_SLOT void initialize();
        /// Common destructor
    private: ~Core();
        /// Getting the instance of Core (singleton)
    public : static Core *instance();

    // Configurations
        /// Reads configuration (default \c config.cfg), fills myConfigurationParameters,
        /// if no keyword be found at file, fills corresponded parameter by empty string or 0;
        /// \n throws std::exception
        /// Format of configuration file is:\n
        /// \li \b keyword \c value . \n
        /// \n keywords are (case insensitive): \n
        /// \li \b TCPPORT
        /// \li \b UIWEBSERVERDOCROOT
        /// \li \b UIWEBSERVERHTTPADDRESS
        /// \li \b UIWEBSERVERHTTPPORT
        /// \li \b UIWEBSERVERAPPROOT
        /// \li \b UIWEBSERVERCONFIG
        /// \li \b UIWEBSERVERACCESSLOG
    private: /*Q_SLOT*/ void _readConfigurationFile(
            const QString configurationFileName = "config.cfg") throw(std::exception);

    /*// Network
        /// Starts or restarts _myQTcpServer
    private: Q_SLOT void runTcpServer();
        /// Starts or restarts myUiWebServer
    private: Q_SLOT void runUiWebServer();*/

    // Errors
        /// Handler for exceptions, write error-message to log and exit from the program.
        /// emits writeString(), creates \c LastCrashReport.log with message
    public : /*Q_SLOT*/ void fatalError(const QString message);

    // Messaging
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
};
#endif // CORE_H
