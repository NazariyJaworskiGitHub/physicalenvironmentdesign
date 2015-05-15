#include "test_fespacesimplex.h"

#include "FEM/staticconstants.h"
//#include <iostream>

using namespace FEM;

void Test_FESpaceSimplex::test_SimplexElement()
{
//    for(int i=0; i<1; ++i)
//    {
//        for(int j=0; j<1*4; ++j)
//            std::cout << N_SI1(i,j) << " ";
//        std::cout << "\n";
//    }
    QVERIFY(N_SI1(0,0).summands.front().multipliers.front().var == L1 &&
            N_SI1(0,1).summands.front().multipliers.front().var == L2 &&
            N_SI1(0,2).summands.front().multipliers.front().var == L3 &&
            N_SI1(0,3).summands.front().multipliers.front().var == L4);

//    for(int i=0; i<3; ++i)
//    {
//        for(int j=0; j<3*4; ++j)
//            std::cout << N_SI3(i,j) << " ";
//        std::cout << "\n";
//    }
    QVERIFY(N_SI3(0,0).summands.front().multipliers.front().var == L1 &&
            N_SI3(0,3).summands.front().multipliers.front().var == L2 &&
            N_SI3(0,6).summands.front().multipliers.front().var == L3 &&
            N_SI3(0,9).summands.front().multipliers.front().var == L4 &&
            N_SI3(1,1).summands.front().multipliers.front().var == L1 &&
            N_SI3(1,4).summands.front().multipliers.front().var == L2 &&
            N_SI3(1,7).summands.front().multipliers.front().var == L3 &&
            N_SI3(1,10).summands.front().multipliers.front().var == L4 &&
            N_SI3(2,2).summands.front().multipliers.front().var == L1 &&
            N_SI3(2,5).summands.front().multipliers.front().var == L2 &&
            N_SI3(2,8).summands.front().multipliers.front().var == L3 &&
            N_SI3(2,11).summands.front().multipliers.front().var == L4 &&
            N_SI3(0,1).summands.empty());
}
