#include <iostream>
#include <QApplication>

#include "volumeglrender.h"
#include "representativevolumeelement.h"

#include "CLMANAGER/clmanager.h"
#include "CLMANAGER/TESTS/tests_runner.h"

#include <chrono>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::cout << OpenCL::CLManager::instance().printPlatformsInfo();
    std::cout << OpenCL::CLManager::instance().printDevicesInfo();

    run_tests_all();

    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

    RVE64 _RVE;
    _RVE.generateRandomField();
    _RVE.applyGaussianFilter(8);
    //_RVE.applyGaussianFilterCL(8);
    //_RVE.applyCuttingLevel(0.65);

    std::chrono::steady_clock::time_point _t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);

    std::cout << time_span.count() << " seconds" << std::endl;

    VolumeGLRender _render(_RVE.getSize(), _RVE.getData(), NULL);
    _render.printOpenGLInfo();
    _render.initializeGL();
    _render.initializeGLEW();
    _render.setWindowTitle("Volume render");
    _render.resize(800,600);
    _render.show();

    return app.exec();
}

