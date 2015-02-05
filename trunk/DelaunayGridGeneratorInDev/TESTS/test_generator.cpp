#include "test_generator.h"

#include <iostream>

#include "piecewiselinearcomplex.h"

using namespace DelaunayGridGenerator;

void Test_Generator::test()
{
    CommonPlc2D _myCommonPlc2D;

    _myCommonPlc2D.createNode(FEM::Node2D(0.0,0.0));
    _myCommonPlc2D.createNode(FEM::Node2D(1.0,0.0));
    _myCommonPlc2D.createNode(FEM::Node2D(0.0,1.0));
    _myCommonPlc2D.createNode(FEM::Node2D(1.0,1.0));

    _myCommonPlc2D.updateMaxAndMinCoordinates();

    DelaunayGridGenerator2D _myGenerator;

    _myGenerator.constructGrid(&_myCommonPlc2D);
}
