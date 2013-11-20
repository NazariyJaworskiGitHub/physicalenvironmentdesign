#include "core.h"
#include <QTime>
#include <QFile>
Core::Core(int argc, char **argv) :
    QCoreApplication(argc, argv), _isOK(true), _isInitialized(false)
{
}

void Core::init()
{
    if(!_isInitialized)
    {
        readConfigurationFile();
        runTcpServer();
        _isInitialized = true;
    }
}

static Core *singletonCore = nullptr; ///< Extra local object
Core *Core::instance()
{
    if(!singletonCore)
        singletonCore = new Core(__argc,__argv);
    return singletonCore;
}

Core::~Core()
{
    delete _myQTcpServer;
}

void Core::readConfigurationFile()
///< Format is:\n
///< \a \b keyword \i value \n
///< keywords are (case insensitive): \n
///< \a \b TCPPORT
///< \todo
{
    if(!QFile::exists("config.cfg"))
        Core::instance()->fatalError("FATAL ERROR: Can't find 'config.cfg'\n");
    else
    {
        QFile *_configurationFile = new QFile("config.cfg");
        if(!_configurationFile->open(QIODevice::ReadOnly | QIODevice::Text))
            Core::instance()->fatalError("FATAL ERROR: Can't open 'config.cfg'\n");
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
    connect(_myQTcpServer,SIGNAL(newConnection()),
            this, SLOT(onNewServerConnection()));
    if(!_myQTcpServer->listen(QHostAddress::Any,_myTcpServerPort))
            Core::instance()->fatalError("FATAL ERROR: Can't bind to TCP server socket\n");
}
void Core::onNewServerConnection()
{

}

void Core::fatalError(const QString message)
{
    QString str = QTime::currentTime().toString() + " " + message;
    //Q_EMIT SIGNAL_WriteToLog(str);
    _isOK = false;
    quit();
}
