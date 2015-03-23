#ifndef CONSOLERUNNER_H
#define CONSOLERUNNER_H

#include <QThread>

#include "CONSOLE/console.h"

#include "CONSOLE/rvemanager.h"

class ConsoleRunner : public QThread, public Console
{
    Q_OBJECT

    private: RVEManager _RVEManager;

    /// Use start() to execute this
    public : void run() override {
        this->runMainLoop();}

    public : ConsoleRunner(
            std::ostream &outputStream,
            std::istream &inputStream,
            QObject * parent = 0):
        QThread(parent),
        Console(outputStream, inputStream),
        _RVEManager(*this)
    {}

    public : ~ConsoleRunner()
    {
        //quit();
        wait(); // Application will wait all threads befor quit
    }
};

#endif // CONSOLERUNNER_H
