#include "test_polynomial.h"
#include <iostream>
using namespace FEM;

void Test_Polynomial::test_Polynomial()
{
//    // Linear
//    Polynomial N1000 = L1;
//    Polynomial N0100 = L2;
//    Polynomial N0010 = L3;
//    Polynomial N0001 = L4;

//    // Square
//    Polynomial N2000 = 2*L1*L1-L1;
//    Polynomial N0200 = 2*L2*L2-L2;
//    Polynomial N0020 = 2*L3*L3-L3;
//    Polynomial N0002 = 2*L4*L4-L4;
//    Polynomial N1100 = 4*L1*L2;
//    Polynomial N0110 = 4*L2*L3;
//    Polynomial N0011 = 4*L3*L4;
//    Polynomial N1001 = 4*L1*L4;

    Polynomial A = 2*L1*L1-L1+1;
    Polynomial B = 2*L2*3*L3-L2;
    Polynomial C = L2 - L1;
    Polynomial D = A - B - C;

    Polynomial True = 2*L1*L1+1-6*L2*3*L3;

//    std::cout << "A = " << A << "\n";
//    std::cout << "B = " << B << "\n";
//    std::cout << "C = " << C << "\n";
//    std::cout << "D = " << D << "\n";

    QVERIFY(True.summands.size() == D.summands.size() &&
            True.summands.size() == 3 &&
            True.summands.front().compare(D.summands.front()) &&
            True.summands.back().compare(D.summands.back()));
}
