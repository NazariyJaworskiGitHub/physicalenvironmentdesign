/// \author Nazariy Jaworski

#include "core.h"

#include <iostream>

#include <QTime>
#include <QFile>
#include <QRegExp>

#include "version.h"

Core::Core(int argc, char **argv) :
    QCoreApplication(argc, argv), _isInitialized(false)
{
    /// \todo
    //this->setApplicationName("Physical Environment Design Core");
    //this->setApplicationVersion("dev:"); 
}

void Core::init()
{
    if(!_isInitialized)
    {
        readConfigurationFile();

        myLogger = new Logger(this);
        connect(this, SIGNAL(writeString(QString)), myLogger, SLOT(writeToLog(QString)));

        myGuard = new Guard(this);

        //runTcpServer();

        runUiWebServer();

        _isInitialized = true;
        Q_EMIT writeString("Core has been Initialized\n");
        Q_EMIT writeString("Core version is " + QString::number(VERSION_BUILD) + "\n");
    }
}

static Core *singletonCore = nullptr; ///< Extra local object
Core *Core::instance()
{
    if(!singletonCore)
        singletonCore = new Core(__argc,__argv);
    return singletonCore;
}

/// Format is:\n
/// \a \b keyword \i value \n
/// keywords are (case insensitive): \n
/// \a \b TCPPORT
/// \a \b UIWEBSERVERDOCROOT
/// \a \b UIWEBSERVERHTTPADDRESS
/// \a \b UIWEBSERVERHTTPPORT
/// \a \b UIWEBSERVERAPPROOT
/// \a \b UIWEBSERVERCONFIG
/// \a \b UIWEBSERVERACCESSLOG
/// \see \file configurationfilekeywords.h
/// \todo
void Core::readConfigurationFile()
{
    if(!QFile::exists("config.cfg"))
        fatalError("FATAL ERROR: Can't find 'config.cfg'\n");
    else
    {
        QFile *_configurationFile = new QFile("config.cfg");
        if(!_configurationFile->open(QIODevice::ReadOnly | QIODevice::Text))
            fatalError("FATAL ERROR: Can't open 'config.cfg'\n");
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
                if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_TCPPORT),
                            Qt::CaseInsensitive) == 0)
                {
                    // Read second word;
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    // If there is not a number or wrong number, port shall be equal to zero
                    this->myConfigurationParameters.tcpServerPort = _currentWord.toInt();
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERDOCROOT),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.uiWebServerDocRoot = new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.uiWebServerDocRoot,_currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPADDRESS),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.uiWebServerHttpAddress = new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.uiWebServerHttpAddress,_currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERHTTPPORT),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.uiWebServerHttpPort = new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.uiWebServerHttpPort,_currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERAPPROOT),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.uiWebServerAppRoot = new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.uiWebServerAppRoot,_currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERCONFIG),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.uiWebServerConfig = new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.uiWebServerConfig,_currentWord.toStdString().data());
                }
                else if(_currentWord.compare(
                            QString(CONFIGURATION_FILE_KEYWORD_UIWEBSERVERACCESSLOG),
                            Qt::CaseInsensitive) == 0)
                {
                    _currentWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                    this->myConfigurationParameters.uiWebServerAccessLog = new char[_currentWord.length()];
                    strcpy(this->myConfigurationParameters.uiWebServerAccessLog,_currentWord.toStdString().data());
                }
            }
            _configurationFile->close();
            delete _configurationFile;
        }
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

void Core::runUiWebServer()
{
    if(myUiWebServer)
        delete myUiWebServer;
    myUiWebServer = new UiWebServer(this);
    myUiWebServer->startServer();
}

void Core::fatalError(const QString message)
{
    QString _str = QTime::currentTime().toString() + " " + message;
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
    for(auto i : myUserSessions)
        delete i;
    myUserSessions.clear();
    if(myGuard)
        delete myGuard;
    //if(_myQTcpServer)
    //    delete _myQTcpServer;
    if(myUiWebServer)
        delete myUiWebServer;
    if(myLogger)
        delete myLogger;
}
