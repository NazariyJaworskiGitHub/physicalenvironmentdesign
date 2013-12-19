/// \file \author Nazariy Jaworski

#include "core.h"

#include <iostream>

#include <QTime>
#include <QFile>
#include <QRegExp>
#include <QStringList>
#include <QSql>

#include "version.h"

Core::Core() :
    QCoreApplication(__argc, __argv), _isInitialized(false)
{
    /// \todo
    //this->setApplicationName("Physical Environment Design Core");
    //this->setApplicationVersion("dev:"); 
}

void Core::initialize()
{
    if(!_isInitialized)
    {
        try
        {
            _readConfigurationFile();

            myLogger = new Logger(this);
            connect(this, SIGNAL(writeString(QString)),
                    myLogger, SLOT(writeToLog(QString)));
            myLogger->openLogFile();

            myGuard = new Guard(this);
            connect(myGuard, SIGNAL(writeString(QString)),
                    myLogger, SLOT(writeToLog(QString)));
            myGuard->readUserDataFromFile();

            //runTcpServer();

            myUiWebServer = new Ui::Web::UiWebServer(
                        myConfigurationParameters.webServerParameters, this);
            connect(myUiWebServer, SIGNAL(writeString(QString)),
                    myLogger, SLOT(writeToLog(QString)));
            myUiWebServer->startServer();
        }
        catch(std::exception &e)
        {
            fatalError(e.what());
            return;
        }

        _isInitialized = true;
        Q_EMIT writeString("Core has been Initialized\n");
        Q_EMIT writeString("Core version is " + QString::number(VERSION_BUILD) + "\n");
        Q_EMIT writeString("Library paths:\n");
        for(auto i : this->libraryPaths())
            Q_EMIT writeString("\t" + i +"\n");
        Q_EMIT writeString("SQL  Drivers:\n");
        for(auto i : QSqlDatabase::drivers())
            Q_EMIT writeString("\t" + i +"\n");
    }
}

Core *Core::instance()
{
    static Core _instanceOfCore;
    return &_instanceOfCore;
}

void Core::_readConfigurationFile(const QString configurationFileName) throw(std::exception)
{
    if(!QFile::exists(configurationFileName))
    {
        Q_EMIT writeString("ERROR: Can't find " + configurationFileName + "\n");
        throw std::runtime_error("ERROR: Can't find " + configurationFileName.toStdString() + "\n");
    }
    else
    {
        QFile *_configurationFile = new QFile(configurationFileName);
        if(!_configurationFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            Q_EMIT writeString("ERROR: Can't open "+ configurationFileName + "\n");
            throw std::runtime_error("ERROR: Can't open "+ configurationFileName.toStdString() + "\n");
        }
        else
        {
            // Processing configuration file line by line as text stream
            QTextStream _input(_configurationFile);

            QString _currentLine;
            QString _currentWord;
            QRegExp _sep("\\s+");   //skip white-characters
            while(!_input.atEnd())
            {
                _currentLine = _input.readLine();
                // Read first word
                _currentWord = _currentLine.section(_sep,0,0,QString::SectionSkipEmpty);

                /// \todo make it easier
                // Comprasions
                /*if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_TCPPORT),
                            Qt::CaseInsensitive) == 0)
                {
                    // Read second word;
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    // If there is not a number or wrong number,
                    // port shall be equal to zero
                    this->myConfigurationParameters.tcpServerPort = _currentWord.toInt();
                }
                else */if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERDOCROOT),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.webServerParameters.uiWebServerDocRoot =
                            new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.webServerParameters.uiWebServerDocRoot,
                           _currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPADDRESS),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.webServerParameters.uiWebServerHttpAddress =
                            new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.webServerParameters.uiWebServerHttpAddress,
                           _currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPPORT),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.webServerParameters.uiWebServerHttpPort =
                            new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.webServerParameters.uiWebServerHttpPort,
                           _currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERAPPROOT),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.webServerParameters.uiWebServerAppRoot =
                            new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.webServerParameters.uiWebServerAppRoot,
                           _currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERCONFIG),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.webServerParameters.uiWebServerConfig =
                            new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.webServerParameters.uiWebServerConfig,
                           _currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERACCESSLOG),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.webServerParameters.uiWebServerAccessLog =
                            new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.webServerParameters.uiWebServerAccessLog,
                           _currentWord.toStdString().data());
                }
            }
            _configurationFile->close();
            delete _configurationFile;
        }
    }
    if(
            myConfigurationParameters.webServerParameters.uiWebServerAccessLog == nullptr ||
            myConfigurationParameters.webServerParameters.uiWebServerAppRoot == nullptr ||
            myConfigurationParameters.webServerParameters.uiWebServerConfig == nullptr ||
            myConfigurationParameters.webServerParameters.uiWebServerDocRoot == nullptr ||
            myConfigurationParameters.webServerParameters.uiWebServerHttpAddress == nullptr ||
            myConfigurationParameters.webServerParameters.uiWebServerHttpPort == nullptr)
    {
        Q_EMIT writeString("ERROR: There aren't some parameter at the configuration file\n");
        throw std::runtime_error("ERROR: There aren't some parameter at the configuration file\n");
    }
}

/*void Core::runTcpServer()
{
    if(_myQTcpServer)
        delete _myQTcpServer;
    _myQTcpServer = new CommandServer(this);
    if(!_myQTcpServer->listen(QHostAddress::Any,_myTcpServerPort))
            fatalError("FATAL ERROR: Can't bind to TCP server socket\n");
    Q_EMIT writeString("Core TCP Server has been started\n");
}*/

void Core::fatalError(const QString message)
{
    QString _str = QTime::currentTime().toString() + " FATAL " + message;
    QFile _lastCrash("LastCrashReport.log");
    _lastCrash.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream _output(&_lastCrash);
    _output << _str;
    _lastCrash.flush();
    _lastCrash.close();
    std::cerr << _str.toStdString();
    Q_EMIT writeString(_str);
    std::exit(-1);
}

Core::~Core()
{
    /*for(auto i : myUserSessions)
        delete i;
    myUserSessions.clear();*/
    if(myGuard)
        delete myGuard;
    //if(_myQTcpServer)
    //    delete _myQTcpServer;
    if(myUiWebServer)
        delete myUiWebServer;
    if(myLogger)
        delete myLogger;
}
