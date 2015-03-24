#ifndef USERINTERFACEMANAGER
#define USERINTERFACEMANAGER

#include <QApplication>

#include "clmanagergui.h"

namespace UserInterface
{
    class UserInterfaceManager : public QApplication
    {
        Q_OBJECT

        public : UserInterfaceManager(int argc, char *argv[]):
            QApplication(argc, argv)
        {}

        private: UserInterface::CLManagerGUI *_CLManagerSetupForm = nullptr;
        public : Q_SIGNAL void signal_OCLSetupGUIError();
        public : Q_SIGNAL void signal_OCLSetupGUIFinish();
        public : Q_SLOT void OCLSetupGUIStart();
        public : Q_SLOT void OCLSetupGUIFinish(int result);

        public : static UserInterfaceManager &instance();

        public : ~UserInterfaceManager();
    };
}
#endif // USERINTERFACEMANAGER

