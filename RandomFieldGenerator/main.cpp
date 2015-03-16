#include <iostream>
#include <QApplication>

#include "volumeglrender.h"
#include "representativevolumeelement.h"

#include "CLMANAGER/clmanager.h"
#include "TESTS/tests_runner.h"

#include <chrono>

/// \todo replace
//#include <viennacl/compressed_matrix.hpp>
//#include <viennacl/vector.hpp>
//#include <viennacl/linalg/cg.hpp>

/// \todo replace
void constructLocalStiffnessMatrix(
        float length, unsigned size,
        unsigned *A,
        unsigned *B,
        unsigned *C,
        unsigned *D,
        float conduction,
        float **_C // output 4x4
        )
{
    // Calculate [C] = [1 Ax Ay Az]
    //                 [1 Bx By Bz]
    //                 [1 Cx Cy Cz]
    //                 [1 Dx Dy Dz]

    _C[0][0] = 1.0f;
    _C[0][1] = length * A[0] / size ;
    _C[0][2] = length * A[1] / size ;
    _C[0][3] = length * A[2] / size ;

    _C[1][0] = 1.0f;
    _C[1][1] = length * B[0] / size ;
    _C[1][2] = length * B[1] / size ;
    _C[1][3] = length * B[2] / size ;

    _C[2][0] = 1.0f;
    _C[2][1] = length * C[0] / size ;
    _C[2][2] = length * C[1] / size ;
    _C[2][3] = length * C[2] / size ;

    _C[3][0] = 1.0f;
    _C[3][1] = length * D[0] / size ;
    _C[3][2] = length * D[1] / size ;
    _C[3][3] = length * D[2] / size ;

    // Calculate determinant of [C]
    float _det = (_C[1][1]*_C[2][2]*_C[3][3]
            - _C[1][1]*_C[2][3]*_C[3][2]
            - _C[2][1]*_C[1][2]*_C[3][3]
            + _C[2][1]*_C[1][3]*_C[3][2]
            + _C[3][1]*_C[1][2]*_C[2][3]
            - _C[3][1]*_C[1][3]*_C[2][2])

            -(_C[0][1]*_C[2][2]*_C[3][3]
            - _C[0][1]*_C[2][3]*_C[3][2]
            - _C[2][1]*_C[0][2]*_C[3][3]
            + _C[2][1]*_C[0][3]*_C[3][2]
            + _C[3][1]*_C[0][2]*_C[2][3]
            - _C[3][1]*_C[0][3]*_C[2][2])

            +(_C[0][1]*_C[1][2]*_C[3][3]
            - _C[0][1]*_C[1][3]*_C[3][2]
            - _C[1][1]*_C[0][2]*_C[3][3]
            + _C[1][1]*_C[0][3]*_C[3][2]
            + _C[3][1]*_C[0][2]*_C[1][3]
            - _C[3][1]*_C[0][3]*_C[1][2])

            -(_C[0][1]*_C[1][2]*_C[2][3]
            - _C[0][1]*_C[1][3]*_C[2][2]
            - _C[1][1]*_C[0][2]*_C[2][3]
            + _C[1][1]*_C[0][3]*_C[2][2]
            + _C[2][1]*_C[0][2]*_C[1][3]
            - _C[2][1]*_C[0][3]*_C[1][2]);

    // Calculate [B] = [C]^-1 \(1-st row)
    float _B[3][4];
    _B[0][0] =-(_C[2][2]*_C[3][3] - _C[2][3]*_C[3][2] - _C[1][2]*_C[3][3] + _C[1][3]*_C[3][2] + _C[1][2]*_C[2][3] - _C[1][3]*_C[2][2]) / _det;
    _B[0][1] = (_C[2][2]*_C[3][3] - _C[2][3]*_C[3][2] - _C[0][2]*_C[3][3] + _C[0][3]*_C[3][2] + _C[0][2]*_C[2][3] - _C[0][3]*_C[2][2]) / _det;
    _B[0][2] =-(_C[1][2]*_C[3][3] - _C[1][3]*_C[3][2] - _C[0][2]*_C[3][3] + _C[0][3]*_C[3][2] + _C[0][2]*_C[1][3] - _C[0][3]*_C[1][2]) / _det;
    _B[0][3] = (_C[1][2]*_C[2][3] - _C[1][3]*_C[2][2] - _C[0][2]*_C[2][3] + _C[0][3]*_C[2][2] + _C[0][2]*_C[1][3] - _C[0][3]*_C[1][2]) / _det;

    _B[1][0] = (_C[2][1]*_C[3][3] - _C[2][3]*_C[3][1] - _C[1][1]*_C[3][3] + _C[1][3]*_C[3][1] + _C[1][1]*_C[2][3] - _C[1][3]*_C[2][1]) / _det;
    _B[1][1] =-(_C[2][1]*_C[3][3] - _C[2][3]*_C[3][1] - _C[0][1]*_C[3][3] + _C[0][3]*_C[3][1] + _C[0][1]*_C[2][3] - _C[0][3]*_C[2][1]) / _det;
    _B[1][2] = (_C[1][1]*_C[3][3] - _C[1][3]*_C[3][1] - _C[0][1]*_C[3][3] + _C[0][3]*_C[3][1] + _C[0][1]*_C[1][3] - _C[0][3]*_C[1][1]) / _det;
    _B[1][3] =-(_C[1][1]*_C[2][3] - _C[1][3]*_C[2][1] - _C[0][1]*_C[2][3] + _C[0][3]*_C[2][1] + _C[0][1]*_C[1][3] - _C[0][3]*_C[1][1]) / _det;

    _B[2][0] =-(_C[2][1]*_C[3][2] - _C[2][2]*_C[3][1] - _C[1][1]*_C[3][2] + _C[1][2]*_C[3][1] + _C[1][1]*_C[2][2] - _C[1][2]*_C[2][1]) / _det;
    _B[2][1] = (_C[2][1]*_C[3][2] - _C[2][2]*_C[3][1] - _C[0][1]*_C[3][2] + _C[0][2]*_C[3][1] + _C[0][1]*_C[2][2] - _C[0][2]*_C[2][1]) / _det;
    _B[2][2] =-(_C[1][1]*_C[3][2] - _C[1][2]*_C[3][1] - _C[0][1]*_C[3][2] + _C[0][2]*_C[3][1] + _C[0][1]*_C[1][2] - _C[0][2]*_C[1][1]) / _det;
    _B[2][3] = (_C[1][1]*_C[2][2] - _C[1][2]*_C[2][1] - _C[0][1]*_C[2][2] + _C[0][2]*_C[2][1] + _C[0][1]*_C[1][2] - _C[0][2]*_C[1][1]) / _det;

    // Calculate element`s volume
    _det /= 6.0f;

    // Calculate local stiffness matrix
    // [C] = [K] = 1/V * [B]^T * [conduction] * [B]
    _C[0][0] = conduction / _det * (_B[0][0]*_B[0][0] + _B[1][0]*_B[1][0] + _B[2][0]*_B[2][0]);
    _C[0][1] = conduction / _det * (_B[0][0]*_B[0][1] + _B[1][0]*_B[1][1] + _B[2][0]*_B[2][1]);
    _C[0][2] = conduction / _det * (_B[0][0]*_B[0][2] + _B[1][0]*_B[1][2] + _B[2][0]*_B[2][2]);
    _C[0][3] = conduction / _det * (_B[0][0]*_B[0][3] + _B[1][0]*_B[1][3] + _B[2][0]*_B[2][3]);

    _C[1][1] = conduction / _det * (_B[0][1]*_B[0][1] + _B[1][1]*_B[1][1] + _B[2][1]*_B[2][1]);
    _C[1][2] = conduction / _det * (_B[0][1]*_B[0][2] + _B[1][1]*_B[1][2] + _B[2][1]*_B[2][2]);
    _C[1][3] = conduction / _det * (_B[0][1]*_B[0][3] + _B[1][1]*_B[1][3] + _B[2][1]*_B[2][3]);

    _C[2][2] = conduction / _det * (_B[0][2]*_B[0][2] + _B[1][2]*_B[1][2] + _B[2][2]*_B[2][2]);
    _C[2][3] = conduction / _det * (_B[0][2]*_B[0][3] + _B[1][2]*_B[1][3] + _B[2][2]*_B[2][3]);

    _C[3][3] = conduction / _det * (_B[0][3]*_B[0][3] + _B[1][3]*_B[1][3] + _B[2][3]*_B[2][3]);

    _C[1][0] = _C[0][1];
    _C[2][0] = _C[0][2];
    _C[3][0] = _C[0][3];

    _C[2][1] = _C[1][2];
    _C[3][1] = _C[1][3];

    _C[3][2] = _C[2][3];
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::cout << OpenCL::CLManager::instance().printPlatformsInfo();
    std::cout << OpenCL::CLManager::instance().printDevicesInfo();

    run_tests_all();

    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

    RVE128 _RVE;
    _RVE.generateRandomField();
    //_RVE.applyGaussianFilter(32);
    _RVE.applyGaussianFilterCL(16);
    //_RVE.applyCuttingLevel(0.65);

    std::chrono::steady_clock::time_point _t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
    std::cout << time_span.count() << " seconds" << std::endl;

    /// \todo replace
//    float h1 = 1.0f;
//    float h2 = 0.1f;
//    float L = 1.0f;
//    std::vector< std::map< unsigned, float> > _cpuK(128);

    VolumeGLRender _render(_RVE.getSize(), _RVE.getData(), NULL);
    _render.printOpenGLInfo();
    _render.initializeGL();
    _render.initializeGLEW();
    _render.setWindowTitle("Volume render");
    _render.resize(800,600);
    _render.show();

    return app.exec();
}

