#include "logger.h"
#include "core.h"
#include <QDateTime>

Logger::Logger(QObject *parent = 0):
    QObject(parent)
{
    QString _logFileName = "CoreLog.log";
    //QString _logFileName = "CoreLog0.log";
    //for(int _i=1;QFile::exists(_logFileName);++_i)
    //{
    //    _logFileName = "CoreLog" + QString::number(_i) + ".log";
    //}
    _logFile = new QFile(_logFileName);
    if(!_logFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        /// \todo try to avoid this instance call, because it does dependence
        Core::instance()->fatalError("FATAL ERROR: Can't open CoreLogXXX.log");
    }
    else
    {
        _logTextStream = new QTextStream(_logFile);
        writeToLog("Logging has been started\n");
    }
}

void Logger::writeToLog(const QString message) const
{
    if(_logTextStream)
    {
        //(*_logTextStream) << QTime::currentTime().toString() << " " << message;
        QTextStream(_logFile) << QTime::currentTime().toString() << " " << message;
        //qDebug() << "LOGGER: " + QTime::currentTime().toString() << " " << message;
    }
    else
        Q_EMIT writeString("ERROR: log file hasn't been opened\n");
}

Logger::~Logger()
{
    if(_logTextStream)
        delete _logTextStream;
    if(_logFile)
    {
        _logFile->flush();
        _logFile->close();
        delete _logFile;
    }
}
