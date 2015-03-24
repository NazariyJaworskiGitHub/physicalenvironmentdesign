#ifndef CONSOLERUNNER_H
#define CONSOLERUNNER_H

#include <QThread>

#include "representativevolumeelementconsoleinterface.h"
#include "clmanagerconsoleinterface.h"

namespace Controller
{
class ConsoleRunner : public QThread, public Console
{
    Q_OBJECT

    private: RepresentativeVolumeElementConsoleInterface *_RVEManager = nullptr;
    private: CLManagerConsoleInterface *_CLManager = nullptr;

    /// Use start() to execute this
    public : void run() override {
        this->runMainLoop();}

    public : ConsoleRunner(
            std::ostream &outputStream,
            std::istream &inputStream,
            QObject * parent = 0):
        QThread(parent),
        Console(outputStream, inputStream),
        _RVEManager(new RepresentativeVolumeElementConsoleInterface(*this)),
        _CLManager(new CLManagerConsoleInterface(*this))
    {}

    public : ~ConsoleRunner()
    {
        //quit();
        wait(); // Application will wait all threads befor quit
        delete _RVEManager;
    }
};
}

#endif // CONSOLERUNNER_H
