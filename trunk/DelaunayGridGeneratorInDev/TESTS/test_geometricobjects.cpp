#include "test_geometricobjects.h"

#include <iostream>

using namespace DelaunayGridGenerator::GeometricObjects;

void Test_GeometricObjects::test_Icosahedron_creation()
{
    try
    {
        Icosahedron _icosahedron(MathUtils::Node3D(0.0, 0.0, 0.0), 0.0);
    }
    catch (std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    Icosahedron _icosahedron(MathUtils::Node3D(0.0, 0.0, 0.0), 1.0);
    QVERIFY(_icosahedron.getCenter() == MathUtils::Node3D(0.0, 0.0, 0.0));
    QVERIFY(_icosahedron.getRadius() == 1.0);
    QVERIFY(_icosahedron.isReflectedToSphere() == false);
    QVERIFY(_icosahedron.getNodes().size() == 12);
}
