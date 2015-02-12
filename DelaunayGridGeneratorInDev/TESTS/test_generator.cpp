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

    DelaunayGridGenerator2D _myGenerator2D;

    // Note, next lines calls the private functions,
    // it is only for testing, don't use it as example
//    {
//        _myGenerator2D._ptrToPlc = const_cast<CommonPlc2D*>(&_myCommonPlc2D);
//        _myGenerator2D._copyAndWrapPlcNodesToInternalStorage();
//        Edge *_firstAliveFacet = _myGenerator2D._constructFirstFacet();
//        _firstAliveFacet->appendToAliveList(_myGenerator2D._aliveFacetsPtrs);
//        Triangle *_firstElement = _myGenerator2D._constructElement(_firstAliveFacet);
//    }
}
