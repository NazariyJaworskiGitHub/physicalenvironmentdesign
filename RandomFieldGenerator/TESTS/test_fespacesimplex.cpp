#include "test_fespacesimplex.h"
#include <iostream>

using namespace FEM;

void Test_FESpaceSimplex::test_SimplexElement()
{

    SimplexElement<1>::LinearInterpolationFunctions Nu1D;
//    for(int i=0; i<1; ++i)
//    {
//        for(int j=0; j<1*4; ++j)
//            std::cout << Nu1D(i,j) << " ";
//        std::cout << "\n";
//    }
    QVERIFY(Nu1D(0,0).summands.front().multipliers.front().var == L1 &&
            Nu1D(0,1).summands.front().multipliers.front().var == L2 &&
            Nu1D(0,2).summands.front().multipliers.front().var == L3 &&
            Nu1D(0,3).summands.front().multipliers.front().var == L4);
    SimplexElement<3>::LinearInterpolationFunctions Nu3D;
//    for(int i=0; i<3; ++i)
//    {
//        for(int j=0; j<3*4; ++j)
//            std::cout << Nu3D(i,j) << " ";
//        std::cout << "\n";
//    }
    QVERIFY(Nu3D(0,0).summands.front().multipliers.front().var == L1 &&
            Nu3D(0,3).summands.front().multipliers.front().var == L2 &&
            Nu3D(0,6).summands.front().multipliers.front().var == L3 &&
            Nu3D(0,9).summands.front().multipliers.front().var == L4 &&
            Nu3D(1,1).summands.front().multipliers.front().var == L1 &&
            Nu3D(1,4).summands.front().multipliers.front().var == L2 &&
            Nu3D(1,7).summands.front().multipliers.front().var == L3 &&
            Nu3D(1,10).summands.front().multipliers.front().var == L4 &&
            Nu3D(2,2).summands.front().multipliers.front().var == L1 &&
            Nu3D(2,5).summands.front().multipliers.front().var == L2 &&
            Nu3D(2,8).summands.front().multipliers.front().var == L3 &&
            Nu3D(2,11).summands.front().multipliers.front().var == L4 &&
            Nu3D(0,1).summands.empty());
}

