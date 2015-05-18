#include "test_matrix.h"
#include <iostream>
using namespace MathUtils;

void Test_Matrix::test_Matrix()
{
    {
        Matrix::StaticMatrix<float,2,3> _T;
        _T(0,0) = 1;
        _T(0,1) = 2;
        _T(0,2) = 3;
        _T(1,0) = 4;
        _T(1,1) = 5;
        _T(1,2) = 6;
        Matrix::StaticMatrix<float,3,2> _TT = _T.T();
        QVERIFY(_TT(0,0) == 1 &&
                _TT(1,0) == 2 &&
                _TT(2,0) == 3 &&
                _TT(0,1) == 4 &&
                _TT(1,1) == 5 &&
                _TT(2,1) == 6 );
    }
    {
        Matrix::StaticMatrix<float,3,3> _MR;
        _MR(0,0) = 1;
        _MR(0,1) = 2;
        _MR(0,2) = 3;
        _MR(1,0) = -1;
        _MR(1,1) = -3;
        _MR(1,2) = -2;
        _MR(2,0) = 3;
        _MR(2,1) = 2;
        _MR(2,2) = 1;
//        float _detR = _MR.determinant();
//        QVERIFY(_detR == 12.0f);
//        _detR = _MR.determinantGaussElimination();
//        QVERIFY(_detR == 12.0f);
        float _detR = _MR.determinant3x3();
        QVERIFY(_detR == 12.0f);
    }
    {
        Matrix::StaticMatrix<float,3,3> _MR;
        _MR(0,0) = 1;
        _MR(0,1) = 2;
        _MR(0,2) = 3;
        _MR(1,0) = -1;
        _MR(1,1) = -2;
        _MR(1,2) = -3;
        _MR(2,0) = 3;
        _MR(2,1) = 2;
        _MR(2,2) = 1;
//        float _detR = _MR.determinant();
//        QVERIFY(_detR == 0.0f);
//        _detR = _MR.determinantGaussElimination();
//        QVERIFY(_detR == 0.0f);
        float _detR = _MR.determinant3x3();
        QVERIFY(_detR == 0.0f);
    }
    {
        Matrix::DynamicMatrix<float> _MRD(3,3);
        _MRD(0,0) = 1;
        _MRD(0,1) = 2;
        _MRD(0,2) = 3;
        _MRD(1,0) = -1;
        _MRD(1,1) = -3;
        _MRD(1,2) = -2;
        _MRD(2,0) = 3;
        _MRD(2,1) = 2;
        _MRD(2,2) = 1;
//        float _detRD = _MRD.determinant();
//        QVERIFY(_detRD == 12.0f);
//        _detRD = _MRD.determinantGaussElimination();
//        QVERIFY(_detRD == 12.0f);
        float _detRD = _MRD.determinant3x3();
        QVERIFY(_detRD == 12.0f);
    }
    {
        Matrix::StaticMatrix<float,4,3> A;
        A(0,0) = 1;
        A(0,1) = 2;
        A(0,2) = 3;
        A(1,0) = -1;
        A(1,1) = -3;
        A(1,2) = -2;
        A(2,0) = 0;
        A(2,1) = 1;
        A(2,2) = 0;
        A(3,0) = 1;
        A(3,1) = 0;
        A(3,2) = 1;
        Matrix::StaticMatrix<float,3,2> B;
        B(0,0) = 2;
        B(0,1) = 1;
        B(1,0) = 1;
        B(1,1) = 3;
        B(2,0) = 2;
        B(2,1) = 4;
        Matrix::StaticMatrix<float,4,2> CTrue;
        CTrue(0,0) = 10;
        CTrue(0,1) = 19;
        CTrue(1,0) = -9;
        CTrue(1,1) = -18;
        CTrue(2,0) = 1;
        CTrue(2,1) = 3;
        CTrue(3,0) = 4;
        CTrue(3,1) = 5;
//        std::cout << "static : ";
        Matrix::StaticMatrix<float,4,2> C = A * B;
        float _maxError = 0.0f;
        for(int i=0; i<4; ++i)
            for(int j=0; j<2; ++j)
        {
            float err = std::fabs(C(i,j)-CTrue(i,j));
            err /= CTrue(i,j);
            if(err>_maxError)
                _maxError = err;
        }
        QVERIFY(_maxError < 1e-4f);
    }
    {
        Matrix::DynamicMatrix<float> A(4,3);
        A(0,0) = 1;
        A(0,1) = 2;
        A(0,2) = 3;
        A(1,0) = -1;
        A(1,1) = -3;
        A(1,2) = -2;
        A(2,0) = 0;
        A(2,1) = 1;
        A(2,2) = 0;
        A(3,0) = 1;
        A(3,1) = 0;
        A(3,2) = 1;
        Matrix::DynamicMatrix<float> B(3,2);
        B(0,0) = 2;
        B(0,1) = 1;
        B(1,0) = 1;
        B(1,1) = 3;
        B(2,0) = 2;
        B(2,1) = 4;
        Matrix::DynamicMatrix<float> CTrue(4,2);
        CTrue(0,0) = 10;
        CTrue(0,1) = 19;
        CTrue(1,0) = -9;
        CTrue(1,1) = -18;
        CTrue(2,0) = 1;
        CTrue(2,1) = 3;
        CTrue(3,0) = 4;
        CTrue(3,1) = 5;
//        std::cout << "dynamic : ";
        Matrix::DynamicMatrix<float> C = A * B;
        float _maxError = 0.0f;
        for(int i=0; i<4; ++i)
            for(int j=0; j<2; ++j)
        {
            float err = std::fabs(C(i,j)-CTrue(i,j));
            err /= CTrue(i,j);
            if(err>_maxError)
                _maxError = err;
        }
        QVERIFY(_maxError < 1e-4f);
    }
    {
        Matrix::StaticMatrix<float,3,2> A;
        A(0,0) =  1;
        A(0,1) = -1;
        A(1,0) =  2;
        A(1,1) = -3;
        A(2,0) =  3;
        A(2,1) = -2;
        Matrix::StaticMatrix<float,2,2> CTrue;
        CTrue(0,0) =  14;
        CTrue(0,1) = -13;
        CTrue(1,0) = -13;
        CTrue(1,1) = -14;
//        std::cout << "static : ";
        Matrix::StaticMatrix<float,2,2> C = A.T() * A;
        float _maxError = 0.0f;
        for(int i=0; i<2; ++i)
            for(int j=0; j<2; ++j)
        {
            float err = std::fabs(C(i,j)-CTrue(i,j));
            err /= CTrue(i,j);
            if(err>_maxError)
                _maxError = err;
        }
        QVERIFY(_maxError < 1e-4f && C.cols() == 2 && C.rows() == 2);
    }
    {
        Matrix::DynamicMatrix<float> A(3,2);
        A(0,0) =  1;
        A(0,1) = -1;
        A(1,0) =  2;
        A(1,1) = -3;
        A(2,0) =  3;
        A(2,1) = -2;
        Matrix::DynamicMatrix<float> CTrue(2,2);
        CTrue(0,0) =  14;
        CTrue(0,1) = -13;
        CTrue(1,0) = -13;
        CTrue(1,1) = -14;
//        std::cout << "dynamic : ";
        Matrix::DynamicMatrix<float> C = A.T() * A;
        float _maxError = 0.0f;
        for(int i=0; i<2; ++i)
            for(int j=0; j<2; ++j)
        {
            float err = std::fabs(C(i,j)-CTrue(i,j));
            err /= CTrue(i,j);
            if(err>_maxError)
                _maxError = err;
        }
        QVERIFY(_maxError < 1e-4f && C.cols() == 2 && C.rows() == 2);
    }
    {
        Matrix::StaticMatrix<float,3,3> C;
        C(0,0) = 0;
        C(0,1) = 0;
        C(0,2) = -0.1;
        C(1,0) = 0.1;
        C(1,1) = 0;
        C(1,2) = -0.1;
        C(2,0) = 0;
        C(2,1) = 0.1;
        C(2,2) = -0.1;
        float det = C.inverse3x3();
        Matrix::StaticMatrix<float,3,3> True;
        True(0,0) = -10;
        True(0,1) = 10;
        True(0,2) = 0;
        True(1,0) = -10;
        True(1,1) = 0;
        True(1,2) = 10;
        True(2,0) = -10;
        True(2,1) = 0;
        True(2,2) = 0;
        float _maxError = 0.0f;
        for(int i=0; i<3; ++i)
            for(int j=0; j<3; ++j)
        {
            float err = std::fabs(C(i,j)-True(i,j));
            if(True(i,j)!=0)err /= True(i,j);
            if(err>_maxError)
                _maxError = err;
        }
        QVERIFY(_maxError < 1e-4f);
        QVERIFY(std::fabs(det + 0.001)/(-0.001) < 1e-4f);
    }
}

void Test_Matrix::test_MatrixExpression()
{
    {
        Matrix::StaticMatrix<float,2,2> A;
        A(0,0) = 1;
        A(0,1) = 2;
        A(1,0) = 3;
        A(1,1) = 4;
        Matrix::StaticMatrix<float,2,1> B;
        B(0,0) = 2;
        B(1,0) = 1;
        Matrix::DynamicMatrix<float> C(2,1);
        C(0,0) = 1;
        C(1,0) = 3;
        Matrix::DynamicMatrix<float> rez = 2 * A * B * C.T() * B;
        Matrix::DynamicMatrix<float> True(2,1);
        True(0,0) = 40;
        True(1,0) = 100;
        float _maxError = 0.0f;
        for(int i=0; i<2; ++i)
            for(int j=0; j<1; ++j)
        {
            float err = std::fabs(rez(i,j)-True(i,j));
            err /= True(i,j);
            if(err>_maxError)
                _maxError = err;
        }
        QVERIFY(_maxError < 1e-4f && rez.cols() == 1 && rez.rows() == 2);
    }
}

void Test_Matrix::test_JacobiMatrix()
{
    float _A[] = {  0,  0,  0};
    float _B[] = {0.1,  0,  0};
    float _C[] = {  0,0.1,  0};
    float _D[] = {  0,  0,0.1};
    FEM::JacobiMatrix Jac(_A,_B,_C,_D);
    float _KTrue[4][4] = {
        {  0,   0, -0.1},
        {0.1,   0, -0.1},
        {  0, 0.1, -0.1}};
    float _maxError = 0.0f;
    for(int i=0; i<3; ++i)
        for(int j=0; j<3; ++j)
        {
            float err = std::fabs(Jac(i,j)-_KTrue[i][j]);
            if(_KTrue[i][j]!=0.0f)err /= _KTrue[i][j];
            if(err>_maxError)
                _maxError = err;
        }
    QVERIFY(_maxError < 1e-4f);
}
