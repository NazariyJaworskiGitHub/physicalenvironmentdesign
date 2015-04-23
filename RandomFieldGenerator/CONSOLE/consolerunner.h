#ifndef CONSOLERUNNER_H
#define CONSOLERUNNER_H

#include <QThread>

#include "LOGGER/logger.h"
#include "representativevolumeelementconsoleinterface.h"
#include "clmanagerconsoleinterface.h"

namespace Controller
{
/// \warning don't make direct usage! it should be nested, but in that case causes Qt error
class _ExecuteScriptGUICommand : public QObject, public ConsoleCommand
{
    Q_OBJECT

    public: _ExecuteScriptGUICommand(Console &console) :
        ConsoleCommand(
        //  "--------------------------------------------------------------------------------"
            "executeScriptGUI",
            "executeScript\n"
            "Run file open dialog for choosing script file\n"
            "Takes no arguments.\n",
            console)
    {
        connect(this, SIGNAL(signal_executeScriptGUIStart()),
                &UserInterface::UserInterfaceManager::instance(),
                SLOT(executeScriptGUIStart()));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_executeScriptGUIError()),
                this, SLOT(executeScriptGUIError()));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_executeScriptGUIFinish(QString)),
                this, SLOT(executeScriptGUIFinish(QString)));
    }
    public: Q_SIGNAL void signal_executeScriptGUIStart();
    public: Q_SLOT void executeScriptGUIError(){
        getConsole().writeToOutput("\rexecuteScriptGUI is already running.\n>");}
    public: Q_SLOT void executeScriptGUIFinish(QString fileName)
    {
        getConsole().writeToOutput("\rexecuteScriptGUI finish.\n>");
        std::stringstream _str;
        _str << "executeScript "
             << fileName.toLocal8Bit().constData();
        getConsole().getOutputStream() << _str.str() << "\n";
        getConsole() << _str.str();
    }
    public: int executeConsoleCommand(const std::vector<std::string> &argv) override
    {
        getConsole().writeToOutput("executeScriptGUI start.\n");
        Q_EMIT signal_executeScriptGUIStart();
        return 0;
    }
};

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
    private: _ExecuteScriptGUICommand * _commandExecuteScriptGUI = nullptr;

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
        *_logger << str + "\n";
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
        _CLManager(new CLManagerConsoleInterface(*this)),
        _commandExecuteScriptGUI(new _ExecuteScriptGUICommand(*this))
    {}

    public : ~ConsoleRunner()
    {
        //quit();
        wait(); // Application will wait all threads befor quit
        delete _commandExecuteScriptGUI;
        delete _CLManager;
        delete _RVEManager;
        delete _logger;
    }
};
}

#endif // CONSOLERUNNER_H
