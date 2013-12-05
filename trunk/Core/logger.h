/// \author Nazariy Jaworski

#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>

/// Provides the logging of all messages to \c CoreLogXXX.log \n
/// where XXX is a nubmer
class Logger : public QObject
{
    Q_OBJECT

        /// File for writing
    private: QFile *_logFile = nullptr;
        /// Text stream corresponding to _logFile
    private: QTextStream *_logTextStream = nullptr;
        /// Common constructor
    public: Logger(QObject *parent);

        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;

        /// Slot for writing data to log file.
    public: Q_SLOT void writeToLog(const QString message) const;

        /// Common destructor
    public: ~Logger();

};

#endif // LOGGER_H
