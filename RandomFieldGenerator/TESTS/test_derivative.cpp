#include "test_derivative.h"
#include <iostream>
#include "FEM/polynomial.h"
#include "matrix.h"
#include "FEM/weakoperator.h"
using namespace FEM;

void Test_Derivative::test_Derivative()
{
    Polynomial f = 2*L1*L1*L2 - 3*L1*L3 + 4*L1 - 1;
    Polynomial df_dL1 = d_dL1 * f;

    Polynomial True = 4*L1*L2 - 3*L3 + 4;

//    std::cout << "f = " << f << "\n";
//    std::cout << "df_dL1 = " << df_dL1 << "\n";

    QVERIFY(True.summands.size() == df_dL1.summands.size() &&
            True.summands.size() == 3 &&
            True.summands.front().compare(df_dL1.summands.front()) &&
            True.summands.back().compare(df_dL1.summands.back()));

    Polynomial dL4_dL1 = d_dL4 * L1;
    QVERIFY(dL4_dL1.summands.empty());
}

void Test_Derivative::test_DerivativeMapped()
{
    MathUtils::Matrix::StaticMatrix<Polynomial,1,4> N;
    N(0,0) = L1;
    N(0,1) = L2;
    N(0,2) = L3;
    N(0,3) = L4;
    MathUtils::Matrix::StaticMatrix<DerivativeMapped,3,1> L;
    L(0,0) = d_dx;
    L(1,0) = d_dy;
    L(2,0) = d_dz;
    MathUtils::Matrix::StaticMatrix<Polynomial,3,4> B;
    B.multiplyT<DerivativeMapped,Polynomial>(L,N);

    QVERIFY(B(0,0).summands.front().coef == 1);
    QVERIFY(B(0,1).summands.empty());
    QVERIFY(B(0,2).summands.empty());
    QVERIFY(B(0,3).summands.front().coef == -1);

    QVERIFY(B(1,0).summands.empty());
    QVERIFY(B(1,1).summands.front().coef == 1);
    QVERIFY(B(1,2).summands.empty());
    QVERIFY(B(1,3).summands.front().coef == -1);

    QVERIFY(B(2,0).summands.empty());
    QVERIFY(B(2,1).summands.empty());
    QVERIFY(B(2,2).summands.front().coef == 1);
    QVERIFY(B(2,3).summands.front().coef == -1);

//    std::cout << "dL1_dx = " << d_dx * L1 << "\n";
//    std::cout << "dL2_dx = " << d_dx * L2 << "\n";
//    std::cout << "dL3_dx = " << d_dx * L3 << "\n";
//    std::cout << "dL4_dx = " << d_dx * L4 << "\n";

//    for(int i=0; i<3; ++i)
//    {
//        for(int j=0; j<4; ++j)
//            std::cout << B(i,j) << " ";
//        std::cout << "\n";
    //    }
}

void Test_Derivative::test_WeakOperator()
{
    MathUtils::Matrix::StaticMatrix<Polynomial,1,4> N;
    N(0,0) = L1;
    N(0,1) = L2;
    N(0,2) = L3;
    N(0,3) = L4;

    MathUtils::Matrix::StaticMatrix<Polynomial,3,4> B;
    B.multiplyT<DerivativeMapped,Polynomial>(Gradient,N);

    QVERIFY(B(0,0).summands.front().coef == 1);
    QVERIFY(B(0,1).summands.empty());
    QVERIFY(B(0,2).summands.empty());
    QVERIFY(B(0,3).summands.front().coef == -1);

    QVERIFY(B(1,0).summands.empty());
    QVERIFY(B(1,1).summands.front().coef == 1);
    QVERIFY(B(1,2).summands.empty());
    QVERIFY(B(1,3).summands.front().coef == -1);

    QVERIFY(B(2,0).summands.empty());
    QVERIFY(B(2,1).summands.empty());
    QVERIFY(B(2,2).summands.front().coef == 1);
    QVERIFY(B(2,3).summands.front().coef == -1);
}
