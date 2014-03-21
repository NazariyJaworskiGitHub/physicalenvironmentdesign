#include "test_gridelement.h"

#include "nodewrapper.h"

using namespace DelaunayGridGenerator;

void Test_GridElement::test()
{
    FEM::Node2D _simpleNodes[] = {{0.0},{1,0},{0,1}};
    WrappedNode2D _wrappedNodes[] = {
        FEM::Node2D(0,0), FEM::Node2D(1,0), FEM::Node2D(0,1)};
    MathUtils::Real _circumRadius1;
    MathUtils::Real _circumRadius2;
    FEM::Node2D _c1 = MathUtils::calculateCircumSphereCenter<FEM::Node2D,2>(
                _simpleNodes,&_circumRadius1);
    WrappedNode2D _c2 = MathUtils::calculateCircumSphereCenter<WrappedNode2D,2>(
                _wrappedNodes,&_circumRadius2);
    QVERIFY(_circumRadius1 == _circumRadius2);
    QVERIFY((_c1-_c2).isNull());
    QVERIFY((_c1-FEM::Node2D(0.5,0.5))[0] < 1e-4);
    QVERIFY(std::fabs(_circumRadius1 - std::sqrt(2)/2) < 1e-4);

    /// \todo 3D

    GridElement<WrappedNode2D,2> _myElement1(
                &_wrappedNodes[0],
                &_wrappedNodes[1],
                &_wrappedNodes[2]);
    QVERIFY((_myElement1.getCircumSphereCenter()-_c2).isNull());
    QVERIFY(_myElement1.getCircumSphereRadius() == _circumRadius2);

    /*GridElement<WrappedNode2D,2> _myElement2(_myElement1);
    //QVERIFY((_myElement2.);
    QVERIFY((_myElement2.getCircumSphereCenter()-_c2).isNull());
    QVERIFY(_myElement2.getCircumSphereRadius() == _circumRadius2);*/
}
