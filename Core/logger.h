#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QFile>
#include <QTextStream>

/// Provides the logging of all messages to \file CoreLogXXX.log \n
/// where XXX is a nubmer
class Logger : public QObject
{
    Q_OBJECT

    private: QFile *_logFile = nullptr;
        ///< file for writing
    private: QTextStream *_logTextStream = nullptr;
        ///< Text stream corresponding to _logFile

    public: Logger(QObject *parent);
        ///< Common constructor

    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger

    public: Q_SLOT void writeToLog(const QString message) const;
        ///< Slot for writing data to log file.

    public: ~Logger();
        ///< Common destructor
};

#endif // LOGGER_H
