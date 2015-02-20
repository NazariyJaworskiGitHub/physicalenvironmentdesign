#include "test_generator.h"

#include <iostream>

#include "piecewiselinearcomplex.h"

using namespace DelaunayGridGenerator;

void Test_Generator::test_1()
{
    // Bad PLC test
    Plc2D _myPlc2D;

    _myPlc2D.createNode(FEM::Node2D(0.0,0.0));
    _myPlc2D.createNode(FEM::Node2D(1.0,0.0));

    _myPlc2D.updateMaxAndMinCoordinates();
    DelaunayGridGenerator2D _myGenerator2D;

    try
    {
        FEM::TriangularGrid *_myGrid2D =
                _myGenerator2D.constructGrid(&_myPlc2D, true);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }

    Plc3D _myPlc3D;

    _myPlc3D.createNode(FEM::Node3D(0.0,0.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(1.0,0.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(0.0,1.0,0.0));

    _myPlc3D.updateMaxAndMinCoordinates();
    DelaunayGridGenerator3D _myGenerator3D;

    try
    {
        FEM::TetrahedralGrid *_myGrid3D =
                _myGenerator3D.constructGrid(&_myPlc3D, true);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
}

void Test_Generator::test_2()
{
    // Element creation test
    Plc2D _myPlc2D;

    _myPlc2D.createNode(FEM::Node2D(0.0,0.0));
    _myPlc2D.createNode(FEM::Node2D(1.0,0.0));
    _myPlc2D.createNode(FEM::Node2D(0.0,1.0));
    _myPlc2D.createNode(FEM::Node2D(1.0,1.0));

    _myPlc2D.updateMaxAndMinCoordinates();

    DelaunayGridGenerator2D _myGenerator2D;
    FEM::TriangularGrid *_myGrid2D =
            _myGenerator2D.constructGrid(&_myPlc2D, true);

    QVERIFY(_myGrid2D->getNodesList()[0] == _myPlc2D.getNodeList()[0] &&
            _myGrid2D->getNodesList()[1] == _myPlc2D.getNodeList()[1] &&
            _myGrid2D->getNodesList()[2] == _myPlc2D.getNodeList()[2] &&
            _myGrid2D->getNodesList()[3] == _myPlc2D.getNodeList()[3] );

    QVERIFY(_myGrid2D->getElementsList()[0].getNodeIndexes()[0] == 0 &&
            _myGrid2D->getElementsList()[0].getNodeIndexes()[1] == 1 &&
            _myGrid2D->getElementsList()[0].getNodeIndexes()[2] == 2 );

    QVERIFY(_myGrid2D->getElementsList()[1].getNodeIndexes()[0] == 2 &&
            _myGrid2D->getElementsList()[1].getNodeIndexes()[1] == 1 &&
            _myGrid2D->getElementsList()[1].getNodeIndexes()[2] == 3 );

    delete (_myGrid2D);

    Plc3D _myPlc3D;

    _myPlc3D.createNode(FEM::Node3D(0.0,0.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(1.0,0.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(0.0,1.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(0.0,0.0,1.0));
    _myPlc3D.createNode(FEM::Node3D(1.0,1.0,1.0));

    _myPlc3D.updateMaxAndMinCoordinates();

    DelaunayGridGenerator3D _myGenerator3D;
    FEM::TetrahedralGrid *_myGrid3D =
            _myGenerator3D.constructGrid(&_myPlc3D, true);

    QVERIFY(_myGrid3D->getNodesList()[0] == _myPlc3D.getNodeList()[0] &&
            _myGrid3D->getNodesList()[1] == _myPlc3D.getNodeList()[1] &&
            _myGrid3D->getNodesList()[2] == _myPlc3D.getNodeList()[2] &&
            _myGrid3D->getNodesList()[3] == _myPlc3D.getNodeList()[3] &&
            _myGrid3D->getNodesList()[4] == _myPlc3D.getNodeList()[4]);

    QVERIFY(_myGrid3D->getElementsList()[0].getNodeIndexes()[0] == 0 &&
            _myGrid3D->getElementsList()[0].getNodeIndexes()[1] == 1 &&
            _myGrid3D->getElementsList()[0].getNodeIndexes()[2] == 2 &&
            _myGrid3D->getElementsList()[0].getNodeIndexes()[3] == 3 );

    QVERIFY(_myGrid3D->getElementsList()[1].getNodeIndexes()[0] == 1 &&
            _myGrid3D->getElementsList()[1].getNodeIndexes()[1] == 2 &&
            _myGrid3D->getElementsList()[1].getNodeIndexes()[2] == 3 &&
            _myGrid3D->getElementsList()[1].getNodeIndexes()[3] == 4 );

    delete (_myGrid3D);
}

void Test_Generator::test_3()
{
    // Delaunay criteria node order test
    Plc3D _myPlc3D;

    _myPlc3D.createNode(FEM::Node3D(0.0,0.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(1.0,0.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(0.75,0.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(0.0,1.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(1.0,1.0,0.0));
    _myPlc3D.createNode(FEM::Node3D(0.5,0.5,0.5));

    _myPlc3D.updateMaxAndMinCoordinates();

    DelaunayGridGenerator3D _myGenerator3D;
    FEM::TetrahedralGrid *_myGrid3D =
            _myGenerator3D.constructGrid(&_myPlc3D, true);

    QVERIFY(_myGrid3D->getElementsList()[0].getNodeIndexes()[0] == 2 &&
            _myGrid3D->getElementsList()[0].getNodeIndexes()[1] == 0 &&
            _myGrid3D->getElementsList()[0].getNodeIndexes()[2] == 5 &&
            _myGrid3D->getElementsList()[0].getNodeIndexes()[3] == 3 );

    QVERIFY(_myGrid3D->getElementsList()[1].getNodeIndexes()[0] == 5 &&
            _myGrid3D->getElementsList()[1].getNodeIndexes()[1] == 2 &&
            _myGrid3D->getElementsList()[1].getNodeIndexes()[2] == 3 &&
            _myGrid3D->getElementsList()[1].getNodeIndexes()[3] == 4 );

    QVERIFY(_myGrid3D->getElementsList()[2].getNodeIndexes()[0] == 5 &&
            _myGrid3D->getElementsList()[2].getNodeIndexes()[1] == 2 &&
            _myGrid3D->getElementsList()[2].getNodeIndexes()[2] == 4 &&
            _myGrid3D->getElementsList()[2].getNodeIndexes()[3] == 1 );

    delete (_myGrid3D);
}

void Test_Generator::test_4()
{
    // Test inner lists update correctness, should not be any exceptions
    DelaunayGridGenerator::Plc2D _myPlc2D;
    _myPlc2D.createNode(FEM::Node2D(0.00, 0.00));
    _myPlc2D.createNode(FEM::Node2D(0.25, 0.25));
    _myPlc2D.createNode(FEM::Node2D(0.00, 0.25));
    _myPlc2D.createNode(FEM::Node2D(0.25, 0.00));
    _myPlc2D.createNode(FEM::Node2D(0.50, 0.00));
    _myPlc2D.createNode(FEM::Node2D(0.00, 0.50));
    _myPlc2D.createNode(FEM::Node2D(0.50, 0.25));
    _myPlc2D.createNode(FEM::Node2D(0.25, 0.50));
    _myPlc2D.createNode(FEM::Node2D(1.00, 0.00));
    _myPlc2D.createNode(FEM::Node2D(0.00, 1.00));
    _myPlc2D.createNode(FEM::Node2D(1.00, 0.25));
    _myPlc2D.createNode(FEM::Node2D(0.25, 1.00));
    _myPlc2D.createNode(FEM::Node2D(0.75, 0.75));
    _myPlc2D.createNode(FEM::Node2D(1.00, 0.75));
    _myPlc2D.createNode(FEM::Node2D(1.00, 1.00));
    _myPlc2D.createNode(FEM::Node2D(0.50, 0.75));
    _myPlc2D.createNode(FEM::Node2D(0.75, 0.50));
    _myPlc2D.updateMaxAndMinCoordinates();
    DelaunayGridGenerator2D _myGenerator2D;
    FEM::TriangularGrid *_myGrid2D =
            _myGenerator2D.constructGrid(&_myPlc2D, false);
    QVERIFY(_myGenerator2D.getAliveNodeList().size() == 0 &&
            _myGenerator2D.getAliveFacetsList().size() == 0 &&
            _myGenerator2D.getDeadNodeList().size() == 17);
    delete (_myGrid2D);
}

void Test_Generator::test_5()
{
    // Intersections test
    Plc3D _myPlc3D;
    int N = 2;
    for(int i=0; i<=N; ++i)
        for(int j=0; j<=N; ++j)
            for(int k=0; k<=N; ++k)
                _myPlc3D.createNode(
                            FEM::Node3D(
                                MathUtils::Real(i)/N,
                                MathUtils::Real(j)/N,
                                MathUtils::Real(k)/N));
    _myPlc3D.updateMaxAndMinCoordinates();

    DelaunayGridGenerator3D _myGenerator3D;
    FEM::TetrahedralGrid *_myGrid3D =
            _myGenerator3D.constructGrid(&_myPlc3D, false);

    QVERIFY(_myGenerator3D.getNodeList().size() == 27 &&
            _myGenerator3D.getDeadNodeList().size() == 27 &&
            _myGenerator3D.getDeadFacetsList().size() == 120 &&
            _myGenerator3D.getElementsList().size() == 48 );

    delete (_myGrid3D);
}


