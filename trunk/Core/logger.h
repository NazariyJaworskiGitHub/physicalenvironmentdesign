/// \file \author Nazariy Jaworski

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

/// Provides the logging of all messages (default to \c CoreLog.log).
/// Use one object per file.
/// \n class is Thread safe.
/// \n throws std::exception
class Logger : public QObject
{
    Q_OBJECT

        /// For thread safe
    private: QMutex _myMutex;
        /// File for writing
    private: QFile *_logFile = nullptr;
        /// Common constructor
    public: Logger(QObject *parent = nullptr);
        /// Open file for logging, logFileName by default is \c CoreLog.log,
        /// \n throws std::exception
    public: void openLogFile(const QString logFileName = "CoreLog.log") throw(std::exception);
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
        /// Slot for writing data to log file,
        /// \n throws std::exception
    public: Q_SLOT void writeToLog(const QString message) throw(std::exception);
        /// Common destructor
    public: ~Logger();

};

#endif // LOGGER_H
