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
                SIGNAL(signal_applyGaussianFilterRVE_T(int,float,float,float,bool,float)),
                this, SLOT(applyGaussianFilterRVE(int,float,float,float,bool,float)));
        connect(this, SIGNAL(signal_applyGaussianFilterRVEDone()),
                &UserInterface::UserInterfaceManager::instance(),
                SIGNAL(signal_applyGaussianFilterRVEDone_T()));
    }
    public: Q_SIGNAL void signal_editRVEGUIStart(RepresentativeVolumeElement* ptrToRVE);
    public: Q_SLOT void editRVEGUIFinish(){
        getConsole().writeToOutput("Edit RVE GUI finish.\n");}
    public: Q_SLOT void editRVEGUIError(){
        getConsole().writeToOutput("Edit RVE GUI is already running.\n");}

    /// See UserInterfaceManager
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
            float intensityFactor);
    public: Q_SIGNAL void signal_applyGaussianFilterRVEDone();

    public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
};

class RepresentativeVolumeElementConsoleInterface : public QObject
{
    Q_OBJECT

    public : std::map<std::string, RepresentativeVolumeElement*> RVEs;

    /// createRVE ----------------------------------------------------------------------------
    public : std::string createRVE(const std::string &name, int size) noexcept;
    private: class _CreateRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _CreateRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
                "createRVE",
                "createRVE <Name> <size>\n"
                "Creates Representative Volume Element (RVE) object in RAM memory.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory,\n"
                "[int]    <size> - the discrete size of RVE. "
                "It should be equal to some power of two.\n",
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
                "deleteRVE",
                "deleteRVE <Name>\n"
                "Deletes Representative Volume Element (RVE) object from RAM memory.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandDeleteRVE = nullptr;

    /// printRVE -----------------------------------------------------------------------------
    public : std::string printRVE() noexcept;
    private: class _PrintRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _PrintRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
                "printRVE",
                "createRVE\n"
                "Prints existing in RAM memory Representative Volume "
                "Element (RVE) objects in\n"
                "format: [<index>]: <name> <size>.\n"
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
                "cleanUnMaskedRVE",
                "cleanUnMaskedRVE <Name> <filler>\n"
                "Clean un masked elements RVE by set them to <filler>.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[float]  <filler> - (optional) value to fill RVE elements (default = 0).\n",
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
                "addRandomNoiseRVE",
                "addRandomNoiseRVE <Name>\n"
                "Add random noise to RVE elements that are not masked by RVE mask.\n"
                "Note that RVE can be unnormalized after this call, "
                "if it wasn't cleaned before.\n"
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
            float intensityFactor);
    private: class _applyGaussianFilterRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _applyGaussianFilterRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
                "applyGaussianFilterRVE",
                "applyGaussianFilterRVE <Name> <Radius> <ScaleFactorX> "
                "<ScaleFactorY> <ScaleFactorZ> <useDataAsIntensity> <intensityFactor>\n"
                "Apply Gaussian blur filter to given Representative "
                "Volume Element elements.\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[int]    <Radius> - the radius of filter mask, should be > 0;\n"
                "[float]  <ScaleFactorX> - (optional) the ellipsoid scale factor on X axis of "
                "filter mask, should be > 0 and <= 1;\n"
                "[float]  <ScaleFactorY> - (optional) the ellipsoid scale factor on Y axis of "
                "filter mask, should be > 0 and <= 1;\n"
                "[float]  <ScaleFactorZ> - (optional) the ellipsoid scale factor on Z axis of "
                "filter mask, should be > 0 and <= 1;\n"
                "[bool]   <useDataAsIntensity> - (optional) use RVE elements as intensity "
                "'true' or 'false';\n"
                "[float]  <intensityFactor> - (optional) if <useDataAsIntensity> equal 'true',"
                "use intensity multiply factor.\n",
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
            float coreValue) noexcept;
    private: class _generateOverlappingRandomEllipsoidsIntenseRVECommand : public ConsoleCommand
    {
        private: RepresentativeVolumeElementConsoleInterface &_manager;
        public: _generateOverlappingRandomEllipsoidsIntenseRVECommand(
                RepresentativeVolumeElementConsoleInterface &manager, Console &console) :
            ConsoleCommand(
                "generateOverlappingRandomEllipsoidsIntenseRVE",
                "generateOverlappingRandomEllipsoidsIntenseRVE <Name> <ellipsoidNum> "
                "<minRadius> <maxRadius> <transitionLayerSize> <ScaleFactorX> "
                "<ScaleFactorY> <ScaleFactorZ> <coreValue>\n"
                "Generate overlapping random ellipsoids at unmasked RVE elements\n"
                "Arguments:\n"
                "[string] <Name> - the name of RVE in RAM memory;\n"
                "[int]    <ellipsoidNum> - number of ellipsoids;\n"
                "[int]    <minRadius> - bottom boudn of radius deviation;\n"
                "[int]    <maxRadius> - top bound of radius deviation;\n"
                "[float]  <transitionLayerSize> - (optional) in range [0:1], relative to radius;\n"
                "[float]  <ScaleFactorX> - (optional) the ellipsoid scale factor on X axis of "
                "filter mask, should be > 0 and <= 1;\n"
                "[float]  <ScaleFactorY> - (optional) the ellipsoid scale factor on Y axis of "
                "filter mask, should be > 0 and <= 1;\n"
                "[float]  <ScaleFactorZ> - (optional) the ellipsoid scale factor on Z axis of "
                "filter mask, should be > 0 and <= 1;\n"
                "[float]  <coreValue> - (optional) intensity value of core.\n",
                console),
                _manager(manager){}
        public: int executeConsoleCommand(const std::vector<std::string> &argv) override;
    } *_commandGenerateOverlappingRandomEllipsoidsIntenseRVE = nullptr;

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
                "generateVoronoiRandomCellsRVE",
                "generateVoronoiRandomCellsRVE <Name> <cellNum>\n"
                "Generate Voronoi diagram random cells OpenCL version. It clears all "
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
        _commandCreateRVE(new _CreateRVECommand(*this, console)),
        _commandDeleteRVE(new _DeleteRVECommand(*this, console)),
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
        delete _commandgenerateVoronoiRandomCellsRVE;
    }
};
}

#endif // RVEMANAGER
