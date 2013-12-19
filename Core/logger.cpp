/// \file \author Nazariy Jaworski

#include "logger.h"
#include <QDateTime>
#include <QDebug>
#include <QMutexLocker>

Logger::Logger(QObject *parent):
    QObject(parent)
{
}

void Logger::openLogFile(const QString logFileName) throw(std::exception)
{
    _myMutex.lock();
    if(_logFile)
    {
        _logFile->flush();
        _logFile->close();
        delete _logFile;
    }
    _logFile = new QFile(logFileName);
    if(!_logFile->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        Q_EMIT writeString("ERROR: Can't open " + logFileName + "\n");
        delete _logFile;
        _logFile = nullptr;
        _myMutex.unlock();
        throw std::runtime_error("ERROR: Can't open " + logFileName.toStdString() + "\n");
        return;
    }
    else
    {
        _myMutex.unlock();
        #ifdef QT_DEBUG
            writeToLog("--Working in DEBUG mode--\n");
        #endif //QT_DEBUG
        writeToLog("Logging has been started\n");
    }
}

void Logger::writeToLog(const QString message) throw(std::exception)
{
    // Locks _myMutex while exist, i.e. only within this method
    QMutexLocker _locker(&_myMutex);
    if(_logFile)
        QTextStream(_logFile) << QTime::currentTime().toString() << " " << message;
    else
    {
        Q_EMIT writeString("ERROR: log file hasn't been opened\n");
        throw std::runtime_error("ERROR: log file hasn't been opened\n");
        return;
    }
}

Logger::~Logger()
{
    if(_logFile)
    {
        _logFile->flush();
        _logFile->close();
        delete _logFile;
    }
}
