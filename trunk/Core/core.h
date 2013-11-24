#ifndef CORE_H
#define CORE_H

#include <QCoreApplication>
#include <QTcpServer>

#include "commandserver.h"
#include "guard.h"
#include "logger.h"
#include "uiwebserver.h"

/// Main program class, does all dirty work. \n
/// There is only one instance of this class object per program,
/// object has CommandServer object, which is processing commands
/// from users through TCP port. \n
class Core : public QCoreApplication
{
    Q_OBJECT

    // Local objects
    private: bool _isInitialized;
        ///< should be set to true after succeseful call of Init()
    public : bool isInitialized() const {return _isInitialized;}
        ///< returns the value of _isInitialized

    private: CommandServer *_myQTcpServer = nullptr;
        ///< For TCP communications
    private: qint16 _myTcpServerPort;
        ///< port of TCP Server

    public : Guard *myGuard = nullptr;
    public : UiWebServer *myUiWebServer = nullptr;

    public : Logger *myLogger = nullptr;
        ///< logger of this object

    // Main functions
    private: Core(int argc, char **argv);
        ///< Common constructor
    public : Q_SLOT void init();
        ///< Initialization, run only onse
    private: ~Core();
        ///< Common destructor, don't forget to call init()
    public : static Core *instance();
        ///< Getting the Instance of Core singleton

    // Configurations
    private: Q_SLOT void readConfigurationFile();
        ///< Reads configuration from \file config.cfg

    // Network
    private: Q_SLOT void runTcpServer();
        ///< Starts or restarts _myQTcpServer
    private: Q_SLOT void runUiWebServer();
    ///< Starts or restarts myUiWebServer

    // Errors
    public : Q_SLOT void fatalError(const QString message);
        ///< Handler for exceptions, write error-message to log and exit from the program.
        ///< emits writeString(), creates \file LastCrashReport.log with message

    // Messaging
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // CORE_H
