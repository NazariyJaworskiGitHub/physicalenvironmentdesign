#include <iostream>
#include <iomanip>
#include <QApplication>

#include "UI/userinterfacemanager.h"
#include "UI/clmanagergui.h"
#include "UI/volumeglrender.h"
#include "UI/volumeglrenderrve.h"

#include "representativevolumeelement.h"

#include "CLMANAGER/clmanager.h"
#include "CLMANAGER/viennaclmanager.h"

#include "TESTS/tests_runner.h"

#include <chrono>

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/vector.hpp>
#include <viennacl/linalg/cg.hpp>
#include <viennacl/linalg/bicgstab.hpp>
#include "simulation.h"

#include "CONSOLE/consolerunner.h"
#include "CONSOLE/representativevolumeelementconsoleinterface.h"

#include "LOGGER/logger.h"

#include "UI/xyglrender.h"

#include "constants.h"

int main(int argc, char *argv[])
{
    Controller::ConsoleRunner _consoleRunner(std::cout, std::cin);
    _consoleRunner.start();

//    Log::Logger LogFile("logfile.txt", &_consoleRunner);

    QLocale::setDefault(QLocale::C);
    UserInterface::UserInterfaceManager::instance().setConsoleRunnerLifetime(_consoleRunner);

//    LogFile << "Setup done.";

    ///////////////////////////////////////////////////////////////////////////////////////
    OpenCL::setupViennaCL();

    ///////////////////////////////////////////////////////////////////////////////////////
//    run_tests_all();
//    LogFile << "Tests done.";

    ///////////////////////////////////////////////////////////////////////////////////////
//    OpenCL::CLManager::instance().setCurrentPlatform(0);
//    OpenCL::CLManager::instance().setCurrentDevice(1);
//    viennacl::ocl::switch_context(0);
//    viennacl::ocl::current_context().switch_device(1);

    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

    int size = 64;
    RepresentativeVolumeElement _RVE(size);
//    _RVE.generateBezierCurveIntense(
//                size/2, size/2, size/2, 5, 10, size*0.9, size/24, 0.25f, 0.5f, 0.0f, 0.0f, M_PI/4);
//    _RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                100, 5, 10, size*0.9, 0.9, size/40, 0.25f, 0.0, false);
//    _RVE.generateOverlappingRandomEllipsoidsIntenseCL(
//                100, size/16, size/2, 0.0f, 1.0f, 0.1f, 0.1f, true, 0.0f, 0.0f, M_PI/4);
//    _RVE.generateRandomEllipsoidIntense(size/4, size/4, 0, size/2, size/2, 0.5f,
//                                        1.0f, 0.25f, 0.25f, 0.0f, 0.0f, M_PI/4);

//    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

//    _RVE.applyTwoCutMaskOutside(0.001f, 0.999f);
//    _RVE.cleanUnMaskedData();
    _RVE.addRandomNoise();
    _RVE.applyGaussianFilterCL(size/8);
//    _RVE.applyGaussianFilter(size/4);
//    _RVE.applyGaussianFilter(size/16,1,1,1,true,0.25f);
//    _RVE.applyGaussianFilter(size/64,1,1,1,true,2);
//    _RVE.applyGaussianFilter(size/256,1,1,1,true,8);
//    _RVE.applyGaussianFilterCL(size/4);
//    _RVE.applyGaussianFilterCL(size/16,1,1,1,true,0.25f);
//    _RVE.applyGaussianFilterCL(size/64,1,1,1,true,2);
//    _RVE.applyGaussianFilterCL(size/256,1,1,1,true,8);

//    _RVE.applyTwoCutMaskOutside(0.001f, 0.999f);
//    _RVE.applyRelativeRandomNoise(0.0075f, 0.0f);
//    _RVE.applyGaussianFilter(size/8, 0.25f); /// \todo fix!
//    _RVE.applyGaussianFilter(size/32, 2.0f); /// \todo fix!
//    _RVE.applyGaussianFilterCL(size/8, 1.0f, 1.0f, 1.0f, true, 0.25f);
//    _RVE.applyGaussianFilterCL(size/32, 1.0f, 1.0f, 1.0f, true, 5.0f);

//    _RVE.applyRelativeRandomNoise(0.75f, 0.0f);
//    _RVE.applyGaussianFilter(size/32, 1, 0.1f, 0.1f, false, 1.0f, true, 0, 0, 3.14159f/4);
//    _RVE.applyGaussianFilterCL(size/32, 1, 0.1f, 0.1f, false, 1.0f, true, 0, 0, 3.14159f/4);

//    _RVE.cleanMask();

//    _RVE.generateVoronoiRandomCells(500);
//    _RVE.generateVoronoiRandomCellsCL(50);
//    _RVE.applyTwoCutMaskOutside(0, 0.05f);
//    _RVE.cleanUnMaskedData(0.0f);
//    _RVE.applyTwoCutMaskInside(0, 0.05f);
//    _RVE.cleanUnMaskedData(1.0f);
//    _RVE.cleanMask();
//    _RVE.invertUnMasked();

    std::chrono::steady_clock::time_point _t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
    std::cout << time_span.count() << " seconds" << std::endl;

    ///////////////////////////////////////////////////////////////////////////////////////
//    OpenCL::CLManager::instance().setCurrentPlatform(0);
//    OpenCL::CLManager::instance().setCurrentDevice(0);
//    viennacl::ocl::switch_context(0);
//    viennacl::ocl::current_context().switch_device(0);

    float _conductionMatrix = 237.0f;
    float _conductionPhase = 1500.0f;
//    float _conductionMatrix = 200.0f;
//    float _conductionPhase = 200.0f;

    ///////////////////////////////////////////////////////////////////////////////////////
    std::vector<UserInterface::NodalFunction> _nodalfunctions;
    UserInterface::NodalFunction _arrayEffective{"Eff. Heat Cond."};
    UserInterface::NodalFunction _arrayMinimal{"Min. Heat Cond."};
    UserInterface::NodalFunction _arrayMaximal{"Max. Heat Cond."};
    int _NumExperiments = 25;

    std::ofstream _OutputFile;
    _OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    _OutputFile.open("RVE64_filter8_237_1500");

    for(int i=0; i<=_NumExperiments; ++i)
    {
        std::cout << (float)i / (float)_NumExperiments << "\n";
        // ...

    ////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "assembling and solving SLAE" << std::endl;
    _t1 = std::chrono::steady_clock::now();

    viennacl::compressed_matrix<float>  _K(size*size*size, size*size*size);
    viennacl::vector<float>             _f(size*size*size);
    viennacl::vector<float>             _u(size*size*size);

    std::vector<std::map<long, float>> cpu_sparse_matrix(size*size*size);
    std::vector<float> cpu_loads(size*size*size);
    std::vector<float> cpu_field(size*size*size);

    Simulation::assembleSiffnessMatrix(
                1e-3f, _RVE.getSize(), _RVE.getData(),
                _conductionMatrix, _conductionPhase, 20.0f, 1e6f,
                (float)i / (float)_NumExperiments,
                cpu_sparse_matrix, cpu_loads);
    //    Simulation::assembleSiffnessMatrix(
    //                1.0f, _RVE.getSize(), _RVE.getData(), 1.0f, 1.0f, 20.0f, 1.0f,
    //                cpu_sparse_matrix, cpu_loads);

    viennacl::copy(cpu_sparse_matrix, _K);
    viennacl::copy(cpu_loads.begin(), cpu_loads.end(), _f.begin());

    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::cg_tag(1e-8, 1000));
    //    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::bicgstab_tag());

    viennacl::copy(_u.begin(), _u.end(), cpu_field.data());

    _t2 = std::chrono::steady_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
    std::cout << time_span.count() << " seconds" << std::endl;
//    unsigned long _matrixSize = 0;
//    for(long i=0; i<size*size*size; ++i)
//        _matrixSize += cpu_sparse_matrix[i].size();
//    std::cout << "matrix (elements): " << _matrixSize << std::endl;
    ///////////////////////////////////////////////////////////////////////////////////////

    float _effCoeff;
    float _minCoeff;
    float _maxCoeff;
    Simulation::calculateConductionCoefficient(
        1e-3f, _RVE.getSize(), cpu_field.data(), 20.0f, 1e6f, _effCoeff, _minCoeff, _maxCoeff);
    std::cout << "Matrix conduction coefficient = " << _conductionMatrix <<"\n";
    std::cout << "Phase conduction coefficient = " << _conductionPhase <<"\n";
    std::cout << "Effective conduction coefficient = " << _effCoeff << "\n";
    std::cout << "Minimal conduction coefficient = " << _minCoeff << "\n";
    std::cout << "Maximal conduction coefficient = " << _maxCoeff << "\n";
    ///////////////////////////////////////////////////////////////////////////////////////

        // ...
        _OutputFile << (float)i / (float)_NumExperiments << " " << _minCoeff
                    << " " << _effCoeff  << " " << _maxCoeff << "\n";
        _arrayEffective.nodes.push_back({i*_conductionPhase/(_NumExperiments), _effCoeff});
        _arrayMinimal.nodes.push_back({i*_conductionPhase/(_NumExperiments), _minCoeff});
        _arrayMaximal.nodes.push_back({i*_conductionPhase/(_NumExperiments), _maxCoeff});
    }
    _OutputFile.close();
    _nodalfunctions.push_back(_arrayEffective);
    _nodalfunctions.push_back(_arrayMinimal);
    _nodalfunctions.push_back(_arrayMaximal);
    ///////////////////////////////////////////////////////////////////////////////////////

    UserInterface::VolumeGLRenderRVE _renderRVE(&_RVE, NULL);
    _renderRVE.setInfoString("");
    _renderRVE.resize(800,600);
    _renderRVE.show();

    ///////////////////////////////////////////////////////////////////////////////////////

//    UserInterface::VolumeGLRender _render(
//                _RVE.getSize(), _RVE.getData(), cpu_field.data(), NULL);
//    _render.setBoundingBoxRepresentationSize(1e-3f);
// //    _render.setInfoString("Info string\nLine 2");
//    _render.resize(800,600);
//    _render.show();
    ///////////////////////////////////////////////////////////////////////////////////////

//    std::vector<UserInterface::Function> functions;
//    functions.push_back(UserInterface::Function{"sin",[](float x)->float{return std::sin(x);}});
//    functions.push_back(UserInterface::Function{"cos",[](float x)->float{return std::cos(x);}});
//    functions.push_back(UserInterface::Function{"x*x",[](float x)->float{return x*x;}});

    UserInterface::XYGLRender _render2D(
                nullptr,
                &_nodalfunctions,
                NULL);
    _render2D.resize(800,600);
    _render2D.show();
    ///////////////////////////////////////////////////////////////////////////////////////
    return UserInterface::UserInterfaceManager::instance().exec();
}
