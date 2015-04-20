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

//    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

    int size = 256;
    RepresentativeVolumeElement _RVE(size);
//    _RVE.generateOverlappingRandomEllipsoids(1, 64, 64, 0.4f);
//    _RVE.generateOverlappingRandomEllipsoidsSmoothed(75, size/2, size, 0.5f, 1, 0.1, 0.1);
//    _RVE.generateOverlappingRandomEllipsoids(150, 16, 32, 0.5f, 1.0f, 0.5f, 0.5f);
//    _RVE.generateOverlappingRandomEllipsoidsSmoothed(15, 16, 32, 0.25f);
//    _RVE.generateRandomEllipsoidSmoothed(size/2, size/2, 0, size/2, size/2, 0.25f);

    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

//    _RVE.applyTwoCutMaskOutside(0.001f, 0.999f);
//    _RVE.cleanUnMaskedData();
//    _RVE.addRandomNoise();
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
//    _RVE.applyGaussianFilter(size/8);  /// \todo fix!
//    _RVE.applyGaussianFilterCL(size/32);

//    _RVE.cleanMask();

//    _RVE.generateVoronoiRandomCells(500);
    _RVE.generateVoronoiRandomCellsCL(50);
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
////    OpenCL::CLManager::instance().setCurrentPlatform(0);
////    OpenCL::CLManager::instance().setCurrentDevice(0);
////    viennacl::ocl::switch_context(0);
////    viennacl::ocl::current_context().switch_device(0);

//    std::cout << "assembling and solving SLAE" << std::endl;
//    _t1 = std::chrono::steady_clock::now();

//    viennacl::compressed_matrix<float>  _K(size*size*size, size*size*size);
//    viennacl::vector<float>             _f(size*size*size);
//    viennacl::vector<float>             _u(size*size*size);

//    std::vector<std::map<long, float>> cpu_sparse_matrix(size*size*size);
//    std::vector<float> cpu_loads(size*size*size);

//    Simulation::assembleSiffnessMatrix(
//                1e-3f, _RVE.getSize(), _RVE.getData(), 237.0f, 1500.0f, 20.0f, 1e6f,
//                cpu_sparse_matrix, cpu_loads);
//    //    Simulation::assembleSiffnessMatrix(
//    //                1.0f, _RVE.getSize(), _RVE.getData(), 1.0f, 1.0f, 20.0f, 1.0f,
//    //                cpu_sparse_matrix, cpu_loads);

//    viennacl::copy(cpu_sparse_matrix, _K);
//    viennacl::copy(cpu_loads.begin(), cpu_loads.end(), _f.begin());

//    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::cg_tag(1e-4, 1000));
//    //    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::bicgstab_tag());

//    viennacl::copy(_u.begin(), _u.end(), _RVE.getCuttedData());

//    _t2 = std::chrono::steady_clock::now();
//    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
//    std::cout << time_span.count() << " seconds" << std::endl;
//    unsigned long _matrixSize = 0;
//    for(long i=0; i<size*size*size; ++i)
//        _matrixSize += cpu_sparse_matrix[i].size();
//    std::cout << "matrix (elements): " << _matrixSize << std::endl;

    ///////////////////////////////////////////////////////////////////////////////////////

    UserInterface::VolumeGLRenderRVE _renderRVE(&_RVE, NULL);
    _renderRVE.setBoundingBoxRepresentationSize(1e-3f);
    _renderRVE.setInfoString("");
    _renderRVE.resize(800,600);
    _renderRVE.show();

    ///////////////////////////////////////////////////////////////////////////////////////

//    UserInterface::VolumeGLRender _render(
//                _RVE.getSize(), _RVE.getData(), _RVE.getCuttedData(), NULL);
//    _render.setBoundingBoxRepresentationSize(1e-3f);
//    _render.setInfoString("Info string\nLine 2");
//    _render.resize(800,600);
//    _render.show();
    ///////////////////////////////////////////////////////////////////////////////////////

//    std::vector<UserInterface::Function> functions;
//    functions.push_back(UserInterface::Function{"sin",[](float x)->float{return std::sin(x);}});
//    functions.push_back(UserInterface::Function{"cos",[](float x)->float{return std::cos(x);}});
//    functions.push_back(UserInterface::Function{"x*x",[](float x)->float{return x*x;}});

//    std::vector<UserInterface::NodalFunction> nodalfunctions;
//    UserInterface::NodalFunction array{"array"};
//    for(int i=0; i<100; ++i)
//        array.nodes.push_back({i*0.01f, MathUtils::rand<float>(0,1)});
//    nodalfunctions.push_back(array);

//    UserInterface::XYGLRender _render2D(
//                &functions,
//                &nodalfunctions,
//                NULL);
//    _render2D.resize(800,600);
//    _render2D.show();
//    ///////////////////////////////////////////////////////////////////////////////////////
    return UserInterface::UserInterfaceManager::instance().exec();
}

