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

        private: bool _isExecuteScriptGUIRunnign = false;
        public: Q_SLOT void executeScriptGUIStart();
        public: Q_SIGNAL void signal_executeScriptGUIError();
        public: Q_SIGNAL void signal_executeScriptGUIFinish(QString fileName);

        /// Transitive from widgets to some console
        public :
            Q_SIGNAL void signal_loadRVE_T(QString fileName);
            Q_SIGNAL void signal_loadRVEDone_T();
            Q_SIGNAL void signal_saveRVE_T(QString fileName);
            Q_SIGNAL void signal_saveRVEDone_T();
            Q_SIGNAL void signal_cleanRVE_T();
            Q_SIGNAL void signal_cleanRVEDone_T();
            Q_SIGNAL void signal_normalizeUnMaskedRVE_T();
            Q_SIGNAL void signal_normalizeUnMaskedRVEDone_T();
            Q_SIGNAL void signal_invertUnMaskedRVE_T();
            Q_SIGNAL void signal_invertUnMaskedRVEDone_T();

            Q_SIGNAL void signal_cleanMaskRVE_T();
            Q_SIGNAL void signal_cleanMaskRVEDone_T();
            Q_SIGNAL void signal_cleanUnMaskedRVE_T(float filler);
            Q_SIGNAL void signal_cleanUnMaskedRVEDone_T();
            Q_SIGNAL void signal_applyTwoCutMaskInsideRVE_T(float cutLevelA, float cutLevelB);
            Q_SIGNAL void signal_applyTwoCutMaskInsideRVEDone_T();
            Q_SIGNAL void signal_applyTwoCutMaskOutsideRVE_T(float cutLevelA, float cutLevelB);
            Q_SIGNAL void signal_applyTwoCutMaskOutsideRVEDone_T();

            Q_SIGNAL void signal_addRandomNoiseRVE_T();
            Q_SIGNAL void signal_addRandomNoiseRVEDone_T();

            Q_SIGNAL void signal_applyGaussianFilterRVE_T(
                int discreteRadius,
                float ellipsoidScaleFactorX,
                float ellipsoidScaleFactorY,
                float ellipsoidScaleFactorZ,
                bool useDataAsIntensity,
                float intensityFactor,
                bool useRotations,
                float rotationOX,
                float rotationOY,
                float rotationOZ);
            Q_SIGNAL void signal_applyGaussianFilterRVEDone_T();

            Q_SIGNAL void signal_generateOverlappingRandomEllipsoidsIntenseRVE_T(
                    int ellipsoidNum,
                    int minRadius,
                    int maxRadius,
                    float transitionLayerSize,
                    float ellipsoidScaleFactorX,
                    float ellipsoidScaleFactorY,
                    float ellipsoidScaleFactorZ,
                    bool useRandomRotations,
                    float rotationOX,
                    float rotationOY,
                    float rotationOZ,
                    float coreValue);
            Q_SIGNAL void signal_generateOverlappingRandomEllipsoidsIntenseRVEDone_T();

            Q_SIGNAL void signal_generateVoronoiRandomCellsRVE_T(int cellNum);
            Q_SIGNAL void signal_generateVoronoiRandomCellsRVEDone_T();

        public : static UserInterfaceManager &instance();

        /// Use to bind UserInterfaceManager lifetime to consoleRunner lifetime
        public : void setConsoleRunnerLifetime(
                Controller::ConsoleRunner &consoleRunner) noexcept;

        public : ~UserInterfaceManager();
    };
}
#endif // USERINTERFACEMANAGER

