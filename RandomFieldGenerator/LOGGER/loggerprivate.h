#ifndef LOGGERPRIVATE_H
#define LOGGERPRIVATE_H

#include <fstream>
#include <ctime>
#include <QObject>
//#include <QMutex>

namespace Log
{
    /// It should be hidden! Don't use directly
    class _HiddenWriter : public QObject
    {
        Q_OBJECT

        private: std::ofstream &_logFileStream;
//        private: QMutex _myMutex;

        public : Q_SLOT void _writeMsgToLog(const QString &msg)
        {
//            _myMutex.lock();

            std::time_t _time = std::time(NULL);
            std::tm _timeInfo(*std::localtime(&_time));
            _logFileStream << "[" << _timeInfo.tm_hour << ":"
                           << _timeInfo.tm_min << ":"
                           << _timeInfo.tm_sec << "] "
                           << msg.toStdString() << std::endl;
//            _myMutex.unlock();
        }

        public : _HiddenWriter(std::ofstream &logFileStream) :
            _logFileStream(logFileStream){}
        public : ~_HiddenWriter(){}
    };
}

#endif // LOGGERPRIVATE_H

