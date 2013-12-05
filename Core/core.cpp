#include "core.h"

#include <iostream>

#include <QTime>
#include <QFile>

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

        runTcpServer();

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

void Core::readConfigurationFile()
///< Format is:\n
///< \a \b keyword \i value \n
///< keywords are (case insensitive): \n
///< \a \b TCPPORT
///< \todo
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
            while(!_input.atEnd())
            {
                _currentLine = _input.readLine();
                // Read first word
                _currentWord = _currentLine.section(' ',0,0,QString::SectionSkipEmpty);

                // Comprasions
                if(_currentWord.compare(QString("TCPPORT"),Qt::CaseInsensitive) == 0)
                {
                    // Read second word;
                    _currentWord = _currentLine.section(' ',1,1,QString::SectionSkipEmpty);
                    // If there is not a number or wrong number, port shall be equal to zero
                    this->_myTcpServerPort = _currentWord.toInt();
                }
            }
            _configurationFile->close();
            delete _configurationFile;
        }
    }
}

void Core::runTcpServer()
{
    if(_myQTcpServer)
        delete _myQTcpServer;
    _myQTcpServer = new CommandServer(this);
    if(!_myQTcpServer->listen(QHostAddress::Any,_myTcpServerPort))
            fatalError("FATAL ERROR: Can't bind to TCP server socket\n");
    Q_EMIT writeString("Core TCP Server has been started\n");
}

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
    if(_myQTcpServer)
        delete _myQTcpServer;
    if(myUiWebServer)
        delete myUiWebServer;
    if(myLogger)
        delete myLogger;
}
