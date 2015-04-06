#ifndef USERINTERFACEMANAGER
#define USERINTERFACEMANAGER

#include <QApplication>

#include "clmanagergui.h"
#include "volumeglrenderrve.h"

namespace Controller
{
    class ConsoleRunner;
}

namespace UserInterface
{
    class UserInterfaceManager : public QApplication
    {
        Q_OBJECT

//        private: Controller::ConsoleRunner *_consoleRunner = nullptr;
//        public : Controller::ConsoleRunner *getConsoleRunner() noexcept {return _consoleRunner;}

        private: UserInterfaceManager(int argc, char *argv[]);

        private: UserInterface::CLManagerGUI *_CLManagerSetupForm = nullptr;
        public : Q_SIGNAL void signal_OCLSetupGUIError();
        public : Q_SIGNAL void signal_OCLSetupGUIFinish();
        public : Q_SLOT void OCLSetupGUIStart();
        public : Q_SLOT void OCLSetupGUIFinish(int result);

        private: UserInterface::VolumeGLRenderRVE *_editRVEForm = nullptr;
        public : Q_SIGNAL void signal_editRVEGUIError();
        public : Q_SIGNAL void signal_editRVEGUIFinish();
        public : Q_SLOT void editRVEGUIStart(RepresentativeVolumeElement *ptrToRVE);
        public : Q_SLOT void editRVEGUIFinish();
        public : Q_SIGNAL void signal_applyGaussFltrRVE_T(    // It is transitive to console
                    int discreteRadius,
                    float ellipsoidScaleFactorX,
                    float ellipsoidScaleFactorY,
                    float ellipsoidScaleFactorZ);
        public : Q_SIGNAL void signal_applyGaussFltrRVEDone_T(); // It is transitive from console

        public : static UserInterfaceManager &instance();

        /// Use to bind UserInterfaceManager lifetime to consoleRunner lifetime
        public : void setConsoleRunnerLifetime(
                Controller::ConsoleRunner &consoleRunner) noexcept;

        public : ~UserInterfaceManager();
    };
}
#endif // USERINTERFACEMANAGER

