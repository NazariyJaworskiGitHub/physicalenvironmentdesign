#include "test_derivative.h"
#include <iostream>
#include "FEM/polynomial.h"
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
}
