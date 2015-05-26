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

#include "timer.h"

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

#include "FEM/problem.h"

int main(int argc, char *argv[])
{
    Controller::ConsoleRunner _consoleRunner(std::cout, std::cin);
    _consoleRunner.start();

    QLocale::setDefault(QLocale::C);
    UserInterface::UserInterfaceManager::instance().setConsoleRunnerLifetime(_consoleRunner);

    ///////////////////////////////////////////////////////////////////////////////////////
    OpenCL::setupViennaCL();

    ///////////////////////////////////////////////////////////////////////////////////////
    run_tests_all();
    _consoleRunner.writeToOutput("Tests done\n");

    ///////////////////////////////////////////////////////////////////////////////////////
    Timer _TotalCalculationTimer;
    _TotalCalculationTimer.start();

    float length = 2;
    int size = 32;
    RepresentativeVolumeElement _RVE(size,length);
    Characteristics ch{4, 480, 1.0/3.0, 0, 0};

    Domain RVEDomain(_RVE);
    RVEDomain.addMaterial(0,1,ch);

//    HeatConductionProblem problem(RVEDomain);
////    problem.BCManager.addNeumannBC(LEFT, {100});
////    problem.BCManager.addNeumannBC(RIGHT,{100});
////    problem.BCManager.addDirichletBC(TOP,{20});
////    problem.BCManager.addDirichletBC(BOTTOM,{20});
////    problem.BCManager.addDirichletBC(LEFT, {30});
////    problem.BCManager.addDirichletBC(RIGHT,{20});
//    problem.BCManager.cleanBCs();
//    problem.BCManager.addNeumannBC(LEFT, {100});
//    problem.BCManager.addDirichletBC(RIGHT,{30});
//    //dT = lq/h

//    std::vector<float> temperature;
//    problem.solve(1e-8,1000,temperature);

    ElasticityProblem problem(RVEDomain);
//    problem.BCManager.addDirichletBC(LEFT, {0,0,0});
//    problem.BCManager.DirichletBCs[2]->setVoid(1);
//    problem.BCManager.DirichletBCs[2]->setVoid(2);
    problem.BCManager.addNeumannBC(LEFT, {300,0,0});
    problem.BCManager.NeumannBCs[2]->setVoid(1);
    problem.BCManager.NeumannBCs[2]->setVoid(2);
    problem.BCManager.addDirichletBC(RIGHT,{0,0,0});
    problem.BCManager.DirichletBCs[3]->setVoid(1);
    problem.BCManager.DirichletBCs[3]->setVoid(2);
    // dUx = lF/E  E = lF/dUx
    // v   = |dUy/dUx| = |dUz/dUx|

    std::vector<float> displacement;
    problem.solve(1e-8,1000,displacement);

    std::vector<float> uX;
    std::vector<float> uY;
    std::vector<float> uZ;

    for(unsigned i=0; i<displacement.size()/3; ++i)
    {
        uX.push_back(displacement[i*3+0]);
        uY.push_back(displacement[i*3+1]);
        uZ.push_back(displacement[i*3+2]);

//        std::cout << displacement[i*3+0] << " "
//                  << displacement[i*3+1] << " "
//                  << displacement[i*3+2] << "\n";
    }
    displacement.clear();

    std::cout << "E = " << length*300/std::fabs(uX[0]-uX[size-1]) << "\n";
    std::cout << "v = " << std::fabs(uY[0]-uY[size*(size-1)+size-1])/
            std::fabs(uX[0]-uX[size-1]) << "\n";
    std::cout << "v = " << std::fabs(uZ[0]-uZ[size*size*(size-1)+size*(size-1)+size-1])/
            std::fabs(uX[0]-uX[size-1]) << "\n";

    _TotalCalculationTimer.stop();
    std::cout << "Total: " << _TotalCalculationTimer.getTimeSpanAsString() << " seconds" << std::endl;

//    UserInterface::VolumeGLRender _render(
//                _RVE.getSize(), _RVE.getData(), temperature.data(), NULL);
//    _render.setBoundingBoxRepresentationSize(2);
//    _render.resize(800,600);
//    _render.show();

    UserInterface::VolumeGLRender _renderUX(
                _RVE.getSize(), _RVE.getData(), uX.data(), NULL);
    _renderUX.setBoundingBoxRepresentationSize(2);
    _renderUX.resize(800,600);
    _renderUX.show();

    UserInterface::VolumeGLRender _renderUY(
                _RVE.getSize(), _RVE.getData(), uY.data(), NULL);
    _renderUY.setBoundingBoxRepresentationSize(2);
    _renderUY.resize(800,600);
    _renderUY.show();

    UserInterface::VolumeGLRender _renderUZ(
                _RVE.getSize(), _RVE.getData(), uZ.data(), NULL);
    _renderUZ.setBoundingBoxRepresentationSize(2);
    _renderUZ.resize(800,600);
    _renderUZ.show();

/*    ///////////////////////////////////////////////////////////////////////////////////////
    OpenCL::CLManager::instance().setCurrentPlatform(0);
    OpenCL::CLManager::instance().setCurrentDevice(1);
    viennacl::ocl::switch_context(0);
    viennacl::ocl::current_context().switch_device(1);

    Timer _TotalCalculationTimer;
    _TotalCalculationTimer.start();
    Timer _RVEGenerationTimer;
    _RVEGenerationTimer.start();

    int size = 64;
    RepresentativeVolumeElement _RVE(size);
//    _RVE.generateBezierCurveIntense(
//                size/2, size/2, 0, 7, 40, size*0.5, size/4, 0.2f, 0.8f);
//    _RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                100, 5, 10, size*0.9, 0.9, size/40, 0.25f, 0.0, false);
//    _RVE.generateOverlappingRandomEllipsoidsIntenseCL(
//                100, size/16, size/2, 0.0f, 1.0f, 0.1f, 0.1f, true, 0.0f, 0.0f, M_PI/4);
//    _RVE.generateRandomEllipsoidIntense(size/4, size/4, 0, size/2, size/2, 0.5f,
//                                        1.0f, 0.25f, 0.25f, 0.0f, 0.0f, M_PI/4);

//    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

//    _RVE.applyTwoCutMaskOutside(0.001f, 0.999f);
//    _RVE.cleanUnMaskedData();
//    _RVE.addRandomNoise();
//    _RVE.applyGaussianFilterCL(size/4);
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

    for(int i=1; i<=100; ++i)
    {
        _RVE.generateOverlappingRandomEllipsoidsIntenseCL(
            1, 8, 12, 0.5f, 0.5f, 0.5f, 1.0f, true, 0.0f, 0.0f, 0.0, i/100.0f);
    }
    _RVE.applyGaussianFilterCL(16, 1,1,1, true, 2);
    _RVE.applyGaussianFilterCL(8, 1,1,1, true, 3);
    _RVE.applyGaussianFilterCL(4, 1,1,1, true, 4);
    _RVE.applyGaussianFilterCL(2, 1,1,1, true, 5);

    _RVEGenerationTimer.stop();
    std::cout << _RVEGenerationTimer.getTimeSpanAsString() << " seconds" << std::endl;

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
    _OutputFile.open("RVE64_ellipsoidMultiscale_237_1500.txt");

    for(int i=0; i<=_NumExperiments; ++i)
    {
        std::cout << (float)i / (float)_NumExperiments << "\n";
        // ...

    ////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "assembling and solving SLAE" << std::endl;
    _RVEGenerationTimer.start();

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

    _RVEGenerationTimer.stop();
    std::cout << _RVEGenerationTimer.getTimeSpanAsString() << " seconds" << std::endl;
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

    _TotalCalculationTimer.stop();
    std::cout << "Total: " << _TotalCalculationTimer.getTimeSpanAsString() << " seconds" << std::endl;

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

//    UserInterface::XYGLRender _render2D(
//                nullptr,
//                &_nodalfunctions,
//                NULL);
//    _render2D.resize(800,600);
//    _render2D.show();
    /////////////////////////////////////////////////////////////////////////////////////// */
    return UserInterface::UserInterfaceManager::instance().exec();
}
