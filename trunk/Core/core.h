#ifndef CORE_H
#define CORE_H

#include <QCoreApplication>
#include <QTcpServer>

#include "commandserver.h"

/// Main program class
class Core : public QCoreApplication
{
    Q_OBJECT

    // Local objects
    private: bool _isOK;
        ///< if some fatal error have place, this value should be set to false
    public : bool isOK() const {return _isOK;}
        ///< returns the value of _isOK
    private: bool _isInitialized;
        ///< should be set to true after succeseful call of Init()
    public : bool isInitialized() const {return _isInitialized;}
        ///< returns the value of _isInitialized

    private: CommandServer *_myQTcpServer = nullptr;
        ///< For TCP communications
    private: qint16 _myTcpServerPort;
        ///< port of TCP Server

    // Main functions
    private: Core(int argc, char **argv);
        ///< Common constructor
    public : void init();
        ///< Initialization, run only onse
    private: ~Core();
        ///< Common destructor
    public : static Core *instance();
        ///< Getting the Instance of Core singleton

    // Configurations
    private: Q_SLOT void readConfigurationFile();
        ///< Reads configuration from \file config.cfg

    // Network
    private: Q_SLOT void runTcpServer();
        ///< Starts or restarts QTcpServer *_myQTcpServer
    private: Q_SLOT void onNewServerConnection();
        ///< Processing new Core connections

    // Errors
    public : Q_SLOT void fatalError(const QString message);
    ///< Handler for exceptions, write error-message to log and exit from the program

};

#endif // CORE_H
