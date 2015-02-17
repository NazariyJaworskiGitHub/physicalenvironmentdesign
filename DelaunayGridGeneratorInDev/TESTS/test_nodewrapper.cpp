#include "test_nodewrapper.h"

#include <iostream>

using namespace DelaunayGridGenerator;

void Test_NodeWrapper::test()
{
    WrappedNode3D _n1;
    try
    {
        _n1[100] = 5.0;
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    QVERIFY(_n1[0] == 0.0 && _n1[1] == 0.0 && _n1[2] == 0.0);
    QVERIFY(_n1.isZero());

    _n1[1]=1e-30;
    QVERIFY(!_n1.isZero());

    WrappedNode3D _n2(FEM::Node3D(1.0,2.0,3.0));
    QVERIFY(_n2[0] == 1.0 && _n2[1] == 2.0 && _n2[2] == 3.0);

    WrappedNode3D _n3 = _n2;
    QVERIFY(_n3[0] == 1.0 && _n3[1] == 2.0 && _n3[2] == 3.0 && _n2 == _n3);

    _n1 = _n3;
    QVERIFY(_n1[0] == 1.0 && _n1[1] == 2.0 && _n1[2] == 3.0 && _n1 == _n3);

    _n1+=1.0;
    QVERIFY(_n1[0] == 2.0 && _n1[1] == 3.0 && _n1[2] == 4.0);

    _n1-=2.0;
    QVERIFY(_n1[0] == 0.0 && _n1[1] == 1.0 && _n1[2] == 2.0);

    _n1*=2.0;
    QVERIFY(_n1[0] == 0.0 && _n1[1] == 2.0 && _n1[2] == 4.0);

    _n1/=2.0;
    QVERIFY(_n1[0] == 0.0 && _n1[1] == 1.0 && _n1[2] == 2.0);

    _n1(1.0,1.0,1.0);
    QVERIFY(_n1[0] == 1.0 && _n1[1] == 1.0 && _n1[2] == 1.0);

    _n2(2.0,2.0,2.0);
    _n3 = 1.0 + _n1*2.0 + 3.0*_n2 - 5.0;
    QVERIFY(_n3[0] == 4.0 && _n3[1] == 4.0 && _n3[2] == 4.0);

    _n1(5.0,0.0,3.0);
    _n1.normalize();
    QVERIFY(std::fabs(_n1.length()-1.0)<1e-8);

    _n1(1.0,-6.0,5.0);
    _n2(2.0,-6.0,5.0);
    QVERIFY(std::fabs(FEM::Node3D::distance(_n1,_n2)-1.0)<1e-8);
    QVERIFY(_n1.getMaxValue() == 5.0);
    QVERIFY(_n1.getMinValue() == -6.0);

    _n1(2.0,0.0,-10.0);
    _n2(4.0,0.0,-10.0);
    QVERIFY(std::fabs(_n1.distanceSquare(_n2)-4.0)<1e-8);
    QVERIFY(std::fabs(_n1.dotProduct(_n2)-108.0)<1e-8);

    WrappedNode3D _v1 = _n1;
    QVERIFY(_v1[0] == 2.0 && _v1[1] == 0.0 && _v1[2] == -10.0);
    QVERIFY(_n1.crossProduct(_n2) == FEM::Node3D(0.0,-20.0,0.0));

    try
    {
        WrappedNode4D _v2;
        WrappedNode3D _v3;
        _v2.crossProduct(_v3);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }

    WrappedNode3D _wn1(FEM::Node3D(1e-30,1e25,5));
    _wn1.truncToDiscreteSpace();
    QVERIFY(_wn1[0] == 0.0);
}
