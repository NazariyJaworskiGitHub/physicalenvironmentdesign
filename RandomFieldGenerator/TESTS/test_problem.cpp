#include "test_problem.h"
//#include <iostream>

using namespace FEM;

void Test_Problem::test_HeatConduction_constructLocalStiffnessMatrix()
{
    MathUtils::Matrix::StaticMatrix<float,4,4> K;
    MathUtils::Matrix::StaticMatrix<float,3,3> D;
    D(0,0) = 3; D(0,1) = 0; D(0,2) = 0;
    D(1,0) = 0; D(1,1) = 3; D(1,2) = 0;
    D(2,0) = 0; D(2,1) = 0; D(2,2) = 3;
    float a[] = {0,0,0};
    float b[] = {0.1,0,0};
    float c[] = {0,0.1,0};
    float d[] = {0,0,0.1};
    HeatConductionProblem::localK(a,b,c,d,D,K);
    float KTrue[4][4] = {
        { 0.15f, -0.05f, -0.05f, -0.05f},
        {-0.05f,  0.05f,   0.0f,   0.0f},
        {-0.05f,   0.0f,  0.05f,   0.0f},
        {-0.05f,   0.0f,   0.0f,  0.05f}};
    float maxError = 0.0f;
    for(int i=0; i<4; ++i)
    {
        for(int j=0; j<4; ++j)
        {
//            std::cout << K(i,j) << " ";
            float err = std::fabs(K(i,j)-KTrue[i][j]);
            if(KTrue[i][j]!=0.0f)
                err /= KTrue[i][j];
            if(err>maxError)
                maxError = err;
        }
//        std::cout << "\n";
    }
    QVERIFY(maxError < 1e-4f);
}

/// see http://www.colorado.edu/engineering/CAS/courses.d/AFEM.d/AFEM.Ch09.d/AFEM.Ch09.pdf
void Test_Problem::test_Elasticity_constructLocalStiffnessMatrix()
{
//    MathUtils::Matrix::StaticMatrix<float,12,12> K;
//    MathUtils::Matrix::StaticMatrix<float,6,6> D;
}

