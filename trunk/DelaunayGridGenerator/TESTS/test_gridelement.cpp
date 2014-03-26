#include "test_gridelement.h"

#include <iostream>

#include "nodewrapper.h"

using namespace DelaunayGridGenerator;

void Test_GridElement::test()
{
    FEM::Node2D _simpleNodes2D[] = {{0.0},{1,0},{0,1}};
    WrappedNode2D _wrappedNodes2D[] = {
        FEM::Node2D(0,0), FEM::Node2D(1,0), FEM::Node2D(0,1)};
    MathUtils::Real _circumRadius1;
    MathUtils::Real _circumRadius2;

    FEM::Node2D _c1 = MathUtils::calculateCircumSphereCenter<FEM::Node2D,2>(
                _simpleNodes2D,&_circumRadius1);
    WrappedNode2D _c2 = MathUtils::calculateCircumSphereCenter<WrappedNode2D,2>(
                _wrappedNodes2D,&_circumRadius2);

    QVERIFY(_circumRadius1 == _circumRadius2);
    QVERIFY((_c1-_c2).isNull());
    QVERIFY((_c1-FEM::Node2D(0.5,0.5))[0] < 1e-4);
    QVERIFY(std::fabs(_circumRadius1 - std::sqrt(2)/2) < 1e-4);

    MathUtils::Real _circumRadius3;
    MathUtils::Real _circumRadius4;
    WrappedNode2D _c3 =
            MathUtils::calculateCircumSphereCenterByCayleyMengerDeterminant<WrappedNode2D,2>(
                _wrappedNodes2D,3,&_circumRadius3);
    WrappedNode2D _c4 =
            MathUtils::calculateCircumSphereCenterByCayleyMengerDeterminant<WrappedNode2D,2>(
                _wrappedNodes2D,2,&_circumRadius4);
    QVERIFY(_circumRadius1 == _circumRadius3);
    QVERIFY((_c1-_c3).isNull());
    QVERIFY((_c3-FEM::Node2D(0.5,0.5))[0] < 1e-4);
    QVERIFY(std::fabs(_circumRadius3 - std::sqrt(2)/2) < 1e-4);
    QVERIFY((_c4-FEM::Node2D(0.5,0.0))[0] < 1e-4);
    QVERIFY(std::fabs(_circumRadius4 - 0.5) < 1e-4);

    /// \todo 3D

    int _n[] = {0,1,2};
    QList<WrappedNode2D> _nodesList = {FEM::Node2D(0,0), FEM::Node2D(1,0), FEM::Node2D(0,1)};
    GridElement<WrappedNode2D,2> _myElement1(
                &_nodesList, _n);
    QVERIFY((_myElement1.getCircumSphereCenter()-_c2).isNull());
    QVERIFY(_myElement1.getCircumSphereRadius() == _circumRadius2);

    GridElement<WrappedNode2D,2> _myElement2(_myElement1);
    QVERIFY(_myElement2[0] == _nodesList[0] &&
            _myElement2[1] == _nodesList[1] &&
            _myElement2[2] == _nodesList[2] );
    QVERIFY((_myElement2.getCircumSphereCenter()-_c2).isNull());
    QVERIFY(_myElement2.getCircumSphereRadius() == _circumRadius2);

    WrappedNode3D _wrappedNodes3D[] = {
        FEM::Node3D(0,0,0), FEM::Node3D(1,0,0), FEM::Node3D(0,2,0), FEM::Node3D(0,0,3)};
    bool _result;

    try
    {
        _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D,1);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D,2);
    QVERIFY(!_result);
    _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D,3);
    QVERIFY(!_result);
    _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D,4);
    QVERIFY(!_result);
    WrappedNode3D _wrappedNodes3D_2[] = {
        FEM::Node3D(1,2,3), FEM::Node3D(1,2,3), FEM::Node3D(2,4,6), FEM::Node3D(4,8,12)};
    try
    {
        _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D_2,0);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D_2,2);
    QVERIFY(_result);
    _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D_2,3);
    QVERIFY(_result);
    _result = MathUtils::calculateIsSamePlaneStatus2<WrappedNode3D,3>(_wrappedNodes3D_2,4);
    QVERIFY(_result);
}
