#include <iostream>
#include <QApplication>

#include "representativevolumeelement.h"
#include "volumeglrender.h"

//#include "CLMANAGER/clmanager.h"
//#include "CLMANAGER/TESTS/tests_runner.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    std::cout << OpenCL::CLManager::instance().printPlatformsInfo();
//    std::cout << OpenCL::CLManager::instance().printDevicesInfo();

//    run_tests_all();

    RVE128 _RVE;
    _RVE.applyGaussianFiler(8, 0.60);
    VolumeGLRender _render(_RVE.getSize(), _RVE.getData(), NULL);
    _render.printOpenGLInfo();
    _render.initializeGL();
    _render.initializeGLEW();
    _render.setWindowTitle("Volume render");
    _render.resize(800,600);
    _render.show();

    return app.exec();
}

