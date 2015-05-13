#include "test_matrix.h"
#include <iostream>
using namespace MathUtils;

void Test_Matrix::test_Matrix()
{
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
        float _detR = _MR.determinant();
        QVERIFY(_detR == 12.0f);
        _detR = _MR.determinantGaussElimination();
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
        float _detR = _MR.determinant();
        QVERIFY(_detR == 0.0f);
        _detR = _MR.determinantGaussElimination();
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
        float _detRD = _MRD.determinant();
        QVERIFY(_detRD == 12.0f);
        _detRD = _MRD.determinantGaussElimination();
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
        Matrix::StaticMatrix<float,4,2> C;
        C.multiply(A,B);
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
}


