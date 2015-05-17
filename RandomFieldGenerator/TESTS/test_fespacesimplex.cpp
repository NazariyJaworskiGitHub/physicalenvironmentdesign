#include "test_fespacesimplex.h"

#include "FEM/staticconstants.h"
//#include <iostream>

using namespace FEM;

void Test_FESpaceSimplex::test_SimplexElement()
{
    QVERIFY(N_SI1(0,0).summands.front().multipliers.front().var == L1 &&
            N_SI1(0,1).summands.front().multipliers.front().var == L2 &&
            N_SI1(0,2).summands.front().multipliers.front().var == L3 &&
            N_SI1(0,3).summands.front().multipliers.front().var == L4);

    QVERIFY(gradN_SI1(0,0) ==  1 &&
            gradN_SI1(1,1) ==  1 &&
            gradN_SI1(2,2) ==  1 &&
            gradN_SI1(0,3) == -1 &&
            gradN_SI1(1,3) == -1 &&
            gradN_SI1(2,3) == -1 &&
            gradN_SI1(0,1) ==  0);

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

    QVERIFY(strainN_SI3(0,0) ==  1 &&
            strainN_SI3(0,9) == -1 &&
            strainN_SI3(1,4) ==  1 &&
            strainN_SI3(1,10)== -1 &&
            strainN_SI3(2,8) ==  1 &&
            strainN_SI3(2,11)== -1 &&
            strainN_SI3(3,1) ==  1 &&
            strainN_SI3(3,3) ==  1 &&
            strainN_SI3(3,9) == -1 &&
            strainN_SI3(3,10)== -1 &&
            strainN_SI3(0,1) ==  0);

//    for(int i=0; i<6; ++i)
//    {
//        for(int j=0; j<12; ++j)
//            std::cout << strainN_SI3(i,j) << " ";
//        std::cout << "\n";
//    }
}
