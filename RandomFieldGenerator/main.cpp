#include <iostream>
#include <iomanip>
#include <QApplication>

#include "UI/volumeglrender.h"
#include "representativevolumeelement.h"

#include "CLMANAGER/clmanager.h"
#include "TESTS/tests_runner.h"

#include <chrono>

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/vector.hpp>
#include <viennacl/linalg/cg.hpp>
#include <viennacl/linalg/bicgstab.hpp>
#include "simulation.h"

#include "CONSOLE/consolerunner.h"
#include "CONSOLE/rvemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::cout << OpenCL::CLManager::instance().printPlatformsInfo();
    std::cout << OpenCL::CLManager::instance().printDevicesInfo();

//    run_tests_all();

    ConsoleRunner _consoleRunner(std::cout, std::cin, &app);
    _consoleRunner.start();

    ///////////////////////////////////////////////////////////////////////////////////////
    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

    int size = 64;
    RepresentativeVolumeElement _RVE(size);
    _RVE.generateRandomField();
    //_RVE.applyGaussianFilterCL(32, 1.0f, 0.25f, 0.25f);
    _RVE.applyGaussianFilterCL(8);
    //_RVE.applyCuttingLevel(0.65);

    std::chrono::steady_clock::time_point _t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
    std::cout << time_span.count() << " seconds" << std::endl;

    ///////////////////////////////////////////////////////////////////////////////////////
    std::cout << "assembling and solving SLAE" << std::endl;
    _t1 = std::chrono::steady_clock::now();
    viennacl::compressed_matrix<float>  _K(size*size*size, size*size*size);
    viennacl::vector<float>             _f(size*size*size);
    viennacl::vector<float>             _u(size*size*size);

    std::vector<std::map<long, float>> cpu_sparse_matrix(size*size*size);
    std::vector<float> cpu_loads(size*size*size);

    Simulation::assembleSiffnessMatrix(
                1.0f, _RVE.getSize(), _RVE.getData(), 0.1f, 1.0f, 0.0f, 1e6f,
                cpu_sparse_matrix, cpu_loads);

//    for(int i=0; i<3; ++i)
//    {
//        std::cout << "[";
//        for(int j=0; j<3; ++j)
//            std::cout << std::scientific << cpu_sparse_matrix[i][j] << " ";
//        std::cout << "... ]" << " [" << cpu_loads[i] << "]" << std::endl;
//    }
//    std::cout << "..." << std::endl;
//    for(int i=size-3; i<size; ++i)
//    {
//        std::cout << "[";
//        for(int j=size-3; j<size; ++j)
//            std::cout << std::scientific << cpu_sparse_matrix[i][j] << " ";
//        std::cout << "... ]" << " [" << cpu_loads[i] << "]" << std::endl;
//    }

    viennacl::copy(cpu_sparse_matrix, _K);
    viennacl::copy(cpu_loads.begin(), cpu_loads.end(), _f.begin());

    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::cg_tag());

    viennacl::copy(_u.begin(), _u.end(), _RVE.getCuttedData());
//    for(long i=0; i<size*size*size; ++i)
//        std::cout << "[" << i << "]:" << _RVE.getData()[i] << " ";
    _t2 = std::chrono::steady_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
    std::cout << time_span.count() << " seconds" << std::endl;
    unsigned long _matrixSize = 0;
    for(long i=0; i<size*size*size; ++i)
        _matrixSize += cpu_sparse_matrix[i].size();
    std::cout << "matrix (elements): " << _matrixSize << std::endl;
    ///////////////////////////////////////////////////////////////////////////////////////

//    QLocale::setDefault(QLocale::C);
    UserInterface::VolumeGLRender _render(
                _RVE.getSize(), _RVE.getData(), _RVE.getCuttedData(), NULL);
    _render.setBoundingBoxRepresentationSize(1.0f);
    _render.setInfoString("Info string\nLine 2");
    _render.resize(800,600);
    _render.show();

    ///////////////////////////////////////////////////////////////////////////////////////
    return app.exec();
}

