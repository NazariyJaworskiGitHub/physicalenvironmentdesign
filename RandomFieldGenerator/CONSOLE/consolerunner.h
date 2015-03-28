#ifndef CONSOLERUNNER_H
#define CONSOLERUNNER_H

#include <QThread>

#include "LOGGER/logger.h"
#include "representativevolumeelementconsoleinterface.h"
#include "clmanagerconsoleinterface.h"

namespace Controller
{
/// Runs console in separated thread;
/// Adds usage commands;
/// It shuld be first and main Qt object that controls all other,
/// bind all other objects lifetime to it;
/// It has own logger that copies all input and output into logfile;
class ConsoleRunner : public QThread, public Console
{
    Q_OBJECT

    private: Log::Logger *_logger = nullptr;
    private: RepresentativeVolumeElementConsoleInterface *_RVEManager = nullptr;
    private: CLManagerConsoleInterface *_CLManager = nullptr;

    /// Use start() to execute this
    public : void run() override {
        this->runMainLoop();}

    public : void writeToOutput(const std::string &str) noexcept override
    {
        *_logger << str;
        this->_outputStream << str;
    }

    public : void echoInput(const std::string &str) noexcept override
    {
        *_logger << str;
    }

    public : ConsoleRunner(
            std::ostream &outputStream,
            std::istream &inputStream,
            const std::string logFileName = "ConsoleLogFile.log",
            QObject * parent = 0):
        QThread(parent),
        Console(outputStream, inputStream),
        _logger(new Log::Logger(logFileName, this)),
        _RVEManager(new RepresentativeVolumeElementConsoleInterface(*this)),
        _CLManager(new CLManagerConsoleInterface(*this))
    {}

    public : ~ConsoleRunner()
    {
        //quit();
        wait(); // Application will wait all threads befor quit
        delete _CLManager;
        delete _RVEManager;
        delete _logger;
    }
};
}

#endif // CONSOLERUNNER_H
