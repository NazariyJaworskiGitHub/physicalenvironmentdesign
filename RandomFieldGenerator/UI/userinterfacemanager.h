#ifndef USERINTERFACEMANAGER
#define USERINTERFACEMANAGER

#include <QApplication>

#include "clmanagergui.h"

namespace Controller
{
    class ConsoleRunner;
}

namespace UserInterface
{
    class UserInterfaceManager : public QApplication
    {
        Q_OBJECT

        private: UserInterfaceManager(int argc, char *argv[]);

        private: UserInterface::CLManagerGUI *_CLManagerSetupForm = nullptr;
        public : Q_SIGNAL void signal_OCLSetupGUIError();
        public : Q_SIGNAL void signal_OCLSetupGUIFinish();
        public : Q_SLOT void OCLSetupGUIStart();
        public : Q_SLOT void OCLSetupGUIFinish(int result);

        public : static UserInterfaceManager &instance();

        /// Use to bind UserInterfaceManager lifetime to consoleRunner lifetime
        public : void setConsoleRunnerLifetime(
                Controller::ConsoleRunner &consoleRunner) noexcept;

        public : ~UserInterfaceManager();
    };
}
#endif // USERINTERFACEMANAGER

