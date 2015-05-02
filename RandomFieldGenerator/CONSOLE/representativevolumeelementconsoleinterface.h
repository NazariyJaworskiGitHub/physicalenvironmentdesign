#ifndef RVEMANAGER
#define RVEMANAGER

#include <map>

#include "UI/userinterfacemanager.h"    // Include it first to fix OpenGL/GLEW compatibility
#include "console.h"
#include "representativevolumeelement.h"
#include "consolecommand.h"

/// \todo a lot of refactoring
namespace Controller
{

class RepresentativeVolumeElementConsoleInterface;

/// editRVE --------------------------------------------------------------------------------
/// \warning don't make direct usage! it should be nested, but in that case causes Qt error
class _EditRVECommand : public QObject, public ConsoleCommand
{
    Q_OBJECT

    private: RepresentativeVolumeElementConsoleInterface &_manager;
    private: std::string _RVEName;
    public: _EditRVECommand(
            RepresentativeVolumeElementConsoleInterface &manager,
            Console &console) :
        ConsoleCommand(
            "editRVE",
            "editRVE <Name>\n"
            "Call the Graphical User Interface Representative Volume Element edit form.\n"
            "Arguments:\n"
            "[string] <Name> - the name of RVE in RAM memory.\n",
            console),
        _manager(manager)
    {
        connect(this, SIGNAL(signal_editRVEGUIStart(RepresentativeVolumeElement*)),
                &UserInterface::UserInterfaceManager::instance(),
                SLOT(editRVEGUIStart(RepresentativeVolumeElement*)));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_editRVEGUIError()),
                this, SLOT(editRVEGUIError()));
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_editRVEGUIFinish()),
                this, SLOT(editRVEGUIFinish()));

        ///
        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_loadRVE_T(QString)),
                this, SLOT(loadRVE(QString)));
        connect(this, SIGNAL(signal_loadRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_loadRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_saveRVE_T(QString)),
                this, SLOT(saveRVE(QString)));
        connect(this, SIGNAL(signal_saveRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_saveRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_cleanRVE_T()),
                this, SLOT(cleanRVE()));
        connect(this, SIGNAL(signal_cleanRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_cleanRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_normalizeUnMaskedRVE_T()),
                this, SLOT(normalizeUnMaskedRVE()));
        connect(this, SIGNAL(signal_normalizeUnMaskedRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_normalizeUnMaskedRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_invertUnMaskedRVE_T()),
                this, SLOT(invertUnMaskedRVE()));
        connect(this, SIGNAL(signal_invertUnMaskedRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_invertUnMaskedRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_cleanMaskRVE_T()),
                this, SLOT(cleanMaskRVE()));
        connect(this, SIGNAL(signal_cleanMaskRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_cleanMaskRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_cleanUnMaskedRVE_T(float)),
                this, SLOT(cleanUnMaskedRVE(float)));
        connect(this, SIGNAL(signal_cleanUnMaskedRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_cleanUnMaskedRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_applyTwoCutMaskInsideRVE_T(float,float)),
                this, SLOT(applyTwoCutMaskInsideRVE(float,float)));
        connect(this, SIGNAL(signal_applyTwoCutMaskInsideRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_applyTwoCutMaskInsideRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_applyTwoCutMaskOutsideRVE_T(float,float)),
                this, SLOT(applyTwoCutMaskOutsideRVE(float,float)));
        connect(this, SIGNAL(signal_applyTwoCutMaskOutsideRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_applyTwoCutMaskOutsideRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_addRandomNoiseRVE_T()),
                this, SLOT(addRandomNoiseRVE()));
        connect(this, SIGNAL(signal_addRandomNoiseRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_addRandomNoiseRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_applyGaussianFilterRVE_T(int,float,float,float,bool,float,bool,float,float,float)),
                this, SLOT(applyGaussianFilterRVE(int,float,float,float,bool,float,bool,float,float,float)));
        connect(this, SIGNAL(signal_applyGaussianFilterRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_applyGaussianFilterRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_generateOverlappingRandomEllipsoidsIntenseRVE_T(int,int,int,float,float,float,float,bool,float,float,float,float)),
                this, SLOT(generateOverlappingRandomEllipsoidsIntenseRVE(int,int,int,float,float,float,float,bool,float,float,float,float)));
        connect(this, SIGNAL(signal_generateOverlappingRandomEllipsoidsIntenseRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_generateOverlappingRandomEllipsoidsIntenseRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_generateOverlappingRandomBezierCurveIntenseRVE_T(int,int,int,int,float,int,float,float,bool,float,float,float,float)),
                this, SLOT(generateOverlappingRandomBezierCurveIntenseRVE(int,int,int,int,float,int,float,float,bool,float,float,float,float)));
        connect(this, SIGNAL(signal_generateOverlappingRandomBezierCurveIntenseRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_generateOverlappingRandomBezierCurveIntenseRVEDone_T()));

        connect(&UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_generateVoronoiRandomCellsRVE_T(int)),
                this, SLOT(generateVoronoiRandomCellsRVE(int)));
        connect(this, SIGNAL(signal_generateVoronoiRandomCellsRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_generateVoronoiRandomCellsRVEDone_T()));
    }
    public: Q_SIGNAL void signal_editRVEGUIStart(RepresentativeVolumeElement* ptrToRVE);
    public: Q_SLOT void editRVEGUIFinish(){
        getConsole().writeToOutput("\rEdit RVE GUI finish.\n>");}
    public: Q_SLOT void editRVEGUIError(){
        getConsole().writeToOutput("\rEdit RVE GUI is already running.\n>");}

    /// See UserInterfaceManager
    public: Q_SLOT void loadRVE(QString fileName);
    public: Q_SIGNAL void signal_loadRVEDone();
    public: Q_SLOT void saveRVE(QString fileName);
    public: Q_SIGNAL void signal_saveRVEDone();
    public: Q_SLOT void cleanRVE();
    public: Q_SIGNAL void signal_cleanRVEDone();
    public: Q_SLOT void normalizeUnMaskedRVE();
    public: Q_SIGNAL void signal_normalizeUnMaskedRVEDone();
    public: Q_SLOT void invertUnMaskedRVE();
    public: Q_SIGNAL void signal_invertUnMaskedRVEDone();

    public: Q_SLOT void cleanMaskRVE();
    public: Q_SIGNAL void signal_cleanMaskRVEDone();
    public: Q_SLOT void cleanUnMaskedRVE(float filler);
    public: Q_SIGNAL void signal_cleanUnMaskedRVEDone();
    public: Q_SLOT void applyTwoCutMaskInsideRVE(float cutLevelA, float cutLevelB);
    public: Q_SIGNAL void signal_applyTwoCutMaskInsideRVEDone();
    public: Q_SLOT void applyTwoCutMaskOutsideRVE(float cutLevelA, float cutLevelB);
    public: Q_SIGNAL void signal_applyTwoCutMaskOutsideRVEDone();

    public: Q_SLOT void addRandomNoiseRVE();
    public: Q_SIGNAL void signal_addRandomNoiseRVEDone();

    public: Q_SLOT void applyGaussianFilterRVE(
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
    public: Q_SIGNAL void signal_applyGaussianFilterRVEDone();

    public: Q_SLOT void generateOverlappingRandomEllipsoidsIntenseRVE(
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
    public: Q_SIGNAL void signal_generateOverlappingRandomEllipsoidsIntenseRVEDone();

    public: Q_SLOT void generateOverlappingRandomBezierCurveIntenseRVE(
            int curveNum,
            int curveOrder,
            int curveApproximationPoints,
            int discreteLength,
            float minScale,
            int curveRadius,
            float pathDeviation,
            float transitionLayerSize,
            bool useRandomRotations,
            float rotationOX,
            float rotationOY,
            float rotationOZ,
            float coreValue);
    public: Q_SIGNAL void signal_generateOverlappingRandomBezierCurveIntenseRVEDone();

    public: Q_SLOT void generateVoronoiRandomCellsRVE(int cellNum);
    public: Q_SIGNAL void signal_generateVoronoiRandomCellsRVEDone();

    public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
};

class RepresentativeVolumeElementConsoleInterface : public QObject
{
    Q_OBJECT

    public : std::map<std::string, RepresentativeVolumeElement*> RVEs;
    private: Console &_refToConsole;

    /// createRVE ----------------------------------------------------------------------------
    public : std::string createRVE(const std::string &name, int size, float representationSize) noexcept;
    private: class _CreateRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _CreateRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "createRVE",
                "createRVE <Name> <size> <representationSize>\n"
                "Creates Representative Volume Element (RVE) object in RAM memory.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[int]    <size> - the discrete size of RVE, It should be equal to\n"
                "           some power of two;\n"
                "[float]  <representationSize> - (optional) represenation size of given RVE,\n"
                "           default = 1.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandCreateRVE = nullptr;

    /// deleteRVE ----------------------------------------------------------------------------
    public : std::string deleteRVE(const std::string &name) noexcept;
    private: class _DeleteRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _DeleteRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "deleteRVE",
                "deleteRVE <Name>\n"
                "Deletes Representative Volume Element (RVE) object from RAM memory.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandDeleteRVE = nullptr;

    /// saveRVE ----------------------------------------------------------------------------
    public : std::string saveRVE(const std::string &name, const std::string &fileName) noexcept;
    private: class _SaveRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _SaveRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "saveRVE",
                "saveRVE <Name> <fileName>\n"
                "Save Representative Volume Element (RVE) object into <filename>.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[string] <fileName> - the name of the file (recommended extension *.RVE).\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandSaveRVE = nullptr;

    /// loadRVE ----------------------------------------------------------------------------
    public : std::string loadRVE(const std::string &name, const std::string &fileName) noexcept;
    private: class _LoadRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _LoadRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "loadRVE",
                "loadRVE <Name> <fileName>\n"
                "Load Representative Volume Element (RVE) object from <filename>.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[string] <fileName> - the name of the file (recommended extension *.RVE).\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandLoadRVE = nullptr;

    /// printRVE -----------------------------------------------------------------------------
    public : std::string printRVE() noexcept;
    private: class _PrintRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _PrintRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "printRVE",
                "printRVE\n"
                "Prints existing in RAM memory Representative Volume "
                "Element (RVE) objects in\n"
                "format: [<index>]: <name> <size> <representationSize>.\n"
                "Takes no arguments.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandPrintRVE = nullptr;

    _EditRVECommand *_commandEditRVE = nullptr;

    /// cleanRVE -------------------------------------------------------------------------
    public : std::string cleanRVE(const std::string &name) noexcept;
    private: class _cleanRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _cleanRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "cleanRVE",
                "cleanRVE <Name>\n"
                "Clean RVE by set all it elements to 0.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandCleanRVE = nullptr;

    /// cleanUnMaskedRVE ---------------------------------------------------------------------
    public : std::string cleanUnMaskedRVE(const std::string &name, float filler) noexcept;
    private: class _cleanUnMaskedRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _cleanUnMaskedRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "cleanUnMaskedRVE",
                "cleanUnMaskedRVE <Name> <filler>\n"
                "Clean un masked elements RVE by set them to <filler>.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[float]  <filler> - (optional) value to fill RVE elements, default = 0.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandCleanUnMaskedRVE = nullptr;

    /// cleanMaskRVE -------------------------------------------------------------------------
    public : std::string cleanMaskRVE(const std::string &name) noexcept;
    private: class _cleanMaskRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _cleanMaskRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "cleanMaskRVE",
                "cleanMaskRVE <Name>\n"
                "Clean RVE mask.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandCleanMaskRVE = nullptr;

    /// addRandomNoiseRVE -------------------------------------------------------------------
    public : std::string addRandomNoiseRVE(const std::string &name) noexcept;
    private: class _addRandomNoiseRVECommandmmand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _addRandomNoiseRVECommandmmand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "addRandomNoiseRVE",
                "addRandomNoiseRVE <Name>\n"
                "Add random noise to RVE elements that are not masked by RVE mask.\n"
                "Note that RVE can be unnormalized after this call, if it wasn't cleaned before.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandAddRandomNoiseRVE = nullptr;

    /// normalizeUnMaskedRVE -----------------------------------------------------------------
    public : std::string normalizeUnMaskedRVE(const std::string &name) noexcept;
    private: class _normalizeUnMaskedRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _normalizeUnMaskedRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "normalizeUnMaskedRVE",
                "normalizeUnMaskedRVE <Name>\n"
                "Normalize un msked RVE elements.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandNormalizeUnMaskedRVE = nullptr;

    /// invertUnMaskedRVE --------------------------------------------------------------------
    public : std::string invertUnMaskedRVE(const std::string &name) noexcept;
    private: class _invertUnMaskedRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _invertUnMaskedRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "invertUnMaskedRVE",
                "invertUnMaskedRVE <Name>\n"
                "Invert normalized un masked RVE elements.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandInvertUnMaskedRVE = nullptr;

    /// applyGaussianFilterRVE --------------------------------------------------------------
    public : std::string applyGaussianFilterRVE(
            const std::string &name,
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
    private: class _applyGaussianFilterRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _applyGaussianFilterRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "applyGaussianFilterRVE",
                "applyGaussianFilterRVE <Name> <Radius> <ScaleFactorX> <ScaleFactorY>\n"
                "   <ScaleFactorZ> <useDataAsIntensity> <intensityFactor>\n"
                "   <useRotations> <rotationOX> <rotationOY> <rotationOZ>\n"
                "Apply Gaussian blur filter to given Representative Volume Element elements.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[int]    <Radius> - the radius of filter mask, should be > 0;\n"
                "[float]  <ScaleFactorX> - (optional) the ellipsoid scale factor on X axis of\n"
                "               filter mask, should be > 0 and <= 1, default = 1;\n"
                "[float]  <ScaleFactorY> - (optional) the ellipsoid scale factor on Y axis of\n"
                "               filter mask, should be > 0 and <= 1, default = 1;\n"
                "[float]  <ScaleFactorZ> - (optional) the ellipsoid scale factor on Z axis of\n"
                "               filter mask, should be > 0 and <= 1, default = 1;\n"
                "[bool]   <useDataAsIntensity> - (optional) use RVE elements as intensity 'true'\n"
                "               or 'false', default = false;\n"
                "[float]  <intensityFactor> - (optional) if <useDataAsIntensity> equal 'true',\n"
                "               use intensity multiply factor, default = 1;\n"
                "[bool]   <useRotations> - (optional) use filter rotations 'true'\n"
                "               or 'false', default = false. Warning! It change filtering\n"
                "               algorithm and could take a long time;\n"
                "[float]  <rotationOX> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               X axis, should be >= 0 and <= 180 radians, default = 0;\n"
                "[float]  <rotationOY> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               Y axis, should be >= 0 and <= 180 radians, default = 0;\n"
                "[float]  <rotationOZ> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               Z axis, should be >= 0 and <= 180 radians, default = 0.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandApplyGaussianFilterRVE = nullptr;

    /// applyTwoCutMaskInsideRVE ----------------------------------------------------------
    public : std::string applyTwoCutMaskInsideRVE(
            const std::string &name,
            float cutLevelA,
            float cutLevelB) noexcept;
    private: class _applyTwoCutMaskInsideRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _applyTwoCutMaskInsideRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "applyTwoCutMaskInsideRVE",
                "applyTwoCutMaskInsideRVE <Name> <cutLevelA> <cutLevelB>\n"
                "Apply two-cut to mask (inside).\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[float]  <cutLevelA> - bottom cut level;\n"
                "[float]  <cutLevelB> - top cut level;\n"
                " 0.0f <= cutlevelA < cutLevelB <= 1.0f.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandApplyTwoCutMaskInsideRVE = nullptr;

    /// applyTwoCutMaskOutsideRVE ----------------------------------------------------------
    public : std::string applyTwoCutMaskOutsideRVE(
            const std::string &name,
            float cutLevelA,
            float cutLevelB) noexcept;
    private: class _applyTwoCutMaskOutsideRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _applyTwoCutMaskOutsideRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "applyTwoCutMaskOutsideRVE",
                "applyTwoCutMaskOutsideRVE <Name> <cutLevelA> <cutLevelB>\n"
                "Apply two-cut to mask (outside).\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[float]  <cutLevelA> - bottom cut level;\n"
                "[float]  <cutLevelB> - top cut level;\n"
                " 0.0f <= cutlevelA < cutLevelB <= 1.0f.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandApplyTwoCutMaskOutsideRVE = nullptr;

    /// generateOverlappingRandomEllipsoidsIntenseRVE ----------------------------------------
    public : std::string generateOverlappingRandomEllipsoidsIntenseRVE(
            const std::string &name,
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
            float coreValue) noexcept;
    private: class _generateOverlappingRandomEllipsoidsIntenseRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _generateOverlappingRandomEllipsoidsIntenseRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "generateOverlappingRandomEllipsoidsIntenseRVE",
                "generateOverlappingRandomEllipsoidsIntenseRVE <Name> <ellipsoidNum> <minRadius>\n"
                "   <maxRadius> <transitionLayerSize> <ScaleFactorX> <ScaleFactorY>\n"
                "   <ScaleFactorZ> <useRandomRotations> <rotationOX> <rotationOY> <rotationOZ>\n"
                "   <coreValue>\n"
                "Generate overlapping random ellipsoids at unmasked RVE elements\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[int]    <ellipsoidNum> - number of ellipsoids;\n"
                "[int]    <minRadius> - bottom boudn of radius deviation;\n"
                "[int]    <maxRadius> - top bound of radius deviation;\n"
                "[float]  <transitionLayerSize> - (optional) in range [0:1], relative to radius,\n"
                "               default = 1;\n"
                "[float]  <ScaleFactorX> - (optional) the ellipsoid scale factor on X axis of\n"
                "               filter mask, should be > 0 and <= 1, default = 1;\n"
                "[float]  <ScaleFactorY> - (optional) the ellipsoid scale factor on Y axis of\n"
                "               filter mask, should be > 0 and <= 1, default = 1;\n"
                "[float]  <ScaleFactorZ> - (optional) the ellipsoid scale factor on Z axis of\n"
                "               filter mask, should be > 0 and <= 1, default = 1;\n"
                "[bool]   <useRandomRotations> - (optional) use random ellipsoid rotations 'true'\n"
                "               or 'false', default = false - next arguments are used;\n"
                "[float]  <rotationOX> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               X axis, should be >= 0 and <= 180 degrees, default = 0;\n"
                "[float]  <rotationOY> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               Y axis, should be >= 0 and <= 180 degrees, default = 0;\n"
                "[float]  <rotationOZ> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               Z axis, should be >= 0 and <= 180 degrees, default = 0;\n"
                "[float]  <coreValue> - (optional) intensity value of core, default = 1.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandGenerateOverlappingRandomEllipsoidsIntenseRVE = nullptr;

    /// generateOverlappingRandomBezierCurveIntenseRVE ----------------------------------------
    public : std::string generateOverlappingRandomBezierCurveIntenseRVE(
            const std::string &name,
            int curveNum,
            int curveOrder,
            int curveApproximationPoints,
            int discreteLength,
            float minScale,
            int curveRadius,
            float pathDeviation,
            float transitionLayerSize,
            bool useRandomRotations,
            float rotationOX,
            float rotationOY,
            float rotationOZ,
            float coreValue) noexcept;
    private: class _generateOverlappingRandomBezierCurveIntenseRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _generateOverlappingRandomBezierCurveIntenseRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "generateOverlappingRandomBezierCurveIntenseRVE",
                "generateOverlappingRandomBezierCurveIntenseRVE <Name> <curveNum> <curveOrder>\n"
                "   <curveApproximationPoints> <discreteLength> <minScale> <curveRadius>\n"
                "   <pathDeviation> <transitionLayerSize> <useRandomRotations> <rotationOX>\n"
                "   <rotationOY> <rotationOZ> <coreValue>\n"
                "Generate overlapping random ellipsoids at unmasked RVE elements\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[int]    <curveNum> - number of curves;\n"
                "[int]    <curveOrder> - order of Bezier curve [2:8];\n"
                "[int]    <curveApproximationPoints> - number of curve approximation\n"
                "               points [2:100];\n"
                "[int]    <discreteLength> - discrete length of curve [1:RVE size];\n"
                "[float]  <minScale> - minimal scale factor for curves radius deviation (0:1];\n"
                "[int]    <curveRadius> - discrete radius of curve [1:RVE size];\n"
                "[float]  <pathDeviation> - path deviation in Y and Z directions [0:1];\n"
                "[float]  <transitionLayerSize> - (optional) in range [0:1], relative to radius,\n"
                "               default = 1;\n"
                "[bool]   <useRandomRotations> - (optional) use random curves rotations 'true'\n"
                "               or 'false', default = false - next arguments are used;\n"
                "[float]  <rotationOX> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               X axis, should be >= 0 and <= 180 degrees, default = 0;\n"
                "[float]  <rotationOY> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               Y axis, should be >= 0 and <= 180 degrees, default = 0;\n"
                "[float]  <rotationOZ> - (optional) the ellipsoid rotation angle in degrees on\n"
                "               Z axis, should be >= 0 and <= 180 degrees, default = 0;\n"
                "[float]  <coreValue> - (optional) intensity value of core [0:1], default = 1.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandGenerateOverlappingRandomBezierCurveIntenseRVE = nullptr;

    /// generateVoronoiRandomCellsRVE ---------------------------------------------------------
    public : std::string generateVoronoiRandomCellsRVE(
            const std::string &name,
            int cellNum) noexcept;
    private: class _generateVoronoiRandomCellsRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _generateVoronoiRandomCellsRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
            //  "--------------------------------------------------------------------------------"
                "generateVoronoiRandomCellsRVE",
                "generateVoronoiRandomCellsRVE <Name> <cellNum>\n"
                "Generate Voronoi diagram random cells OpenCL version. It clears all\n"
                "previous data\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[int]    <cellNum> - number of cells.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandgenerateVoronoiRandomCellsRVE = nullptr;

    /// Constructor --------------------------------------------------------------------------
    public : RepresentativeVolumeElementConsoleInterface(Console &console):
        _refToConsole(console),
        _commandCreateRVE(new _CreateRVECommand(*this, console)),
        _commandDeleteRVE(new _DeleteRVECommand(*this, console)),
        _commandSaveRVE(new _SaveRVECommand(*this, console)),
        _commandLoadRVE(new _LoadRVECommand(*this, console)),
        _commandPrintRVE(new _PrintRVECommand(*this, console)),
        _commandEditRVE(new _EditRVECommand(*this, console)),
        _commandCleanRVE(new _cleanRVECommand(*this, console)),
        _commandCleanUnMaskedRVE(new _cleanUnMaskedRVECommand(*this, console)),
        _commandCleanMaskRVE(new _cleanMaskRVECommand(*this, console)),
        _commandAddRandomNoiseRVE(new _addRandomNoiseRVECommandmmand(*this, console)),
        _commandNormalizeUnMaskedRVE(new _normalizeUnMaskedRVECommand(*this, console)),
        _commandInvertUnMaskedRVE(new _invertUnMaskedRVECommand(*this, console)),
        _commandApplyGaussianFilterRVE(new _applyGaussianFilterRVECommand(*this, console)),
        _commandApplyTwoCutMaskInsideRVE(new _applyTwoCutMaskInsideRVECommand(*this, console)),
        _commandApplyTwoCutMaskOutsideRVE(
            new _applyTwoCutMaskOutsideRVECommand(*this, console)),
        _commandGenerateOverlappingRandomEllipsoidsIntenseRVE(
            new _generateOverlappingRandomEllipsoidsIntenseRVECommand(*this, console)),
        _commandGenerateOverlappingRandomBezierCurveIntenseRVE(
            new _generateOverlappingRandomBezierCurveIntenseRVECommand(*this, console)),
        _commandgenerateVoronoiRandomCellsRVE(
            new _generateVoronoiRandomCellsRVECommand(*this, console))
        {}

    /// Destructor ---------------------------------------------------------------------------
    public : ~RepresentativeVolumeElementConsoleInterface()
    {
        for(auto _rve: RVEs)
            delete _rve.second;
        delete _commandCreateRVE;
        delete _commandDeleteRVE;
        delete _commandSaveRVE;
        delete _commandLoadRVE;
        delete _commandPrintRVE;
        delete _commandEditRVE;
        delete _commandCleanRVE;
        delete _commandCleanUnMaskedRVE;
        delete _commandCleanMaskRVE;
        delete _commandAddRandomNoiseRVE;
        delete _commandNormalizeUnMaskedRVE;
        delete _commandInvertUnMaskedRVE;
        delete _commandApplyGaussianFilterRVE;
        delete _commandApplyTwoCutMaskInsideRVE;
        delete _commandApplyTwoCutMaskOutsideRVE;
        delete _commandGenerateOverlappingRandomEllipsoidsIntenseRVE;
        delete _commandGenerateOverlappingRandomBezierCurveIntenseRVE;
        delete _commandgenerateVoronoiRandomCellsRVE;
    }
};
}

#endif // RVEMANAGER
