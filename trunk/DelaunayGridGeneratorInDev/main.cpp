#include <simpleglrender.h>

#include "TESTS/tests_runner.h"

int main(int argc, char *argv[])
{
    run_tests_all();

    QApplication app(argc, argv);

    DelaunayGridGenerator::CommonPlc3D *_myCommonPlc3D =
            new DelaunayGridGenerator::CommonPlc3D();

//    /// TEST 1
//    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(1.0,0.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(0.0,1.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,1.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(1.0,1.0,1.0));

    /// TEST 2
    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,0.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(0.0,1.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,1.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(0.5,0.5,0.5));
    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,1.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,0.0,1.0));
    _myCommonPlc3D->createNode(FEM::Node3D(0.0,1.0,1.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,1.0,1.0));

//    /// TEST 3
//    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(1.0,0.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(0.75,0.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(0.0,1.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(1.0,1.0,0.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,1.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(1.0,0.0,1.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(0.0,1.0,1.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(1.0,1.0,1.0));
//    _myCommonPlc3D->createNode(FEM::Node3D(0.5,0.55,0.5));

    _myCommonPlc3D->updateMaxAndMinCoordinates();

//    _myCommonPlc3D->createSegment(0,1);
//    _myCommonPlc3D->createSegment(0,2);
//    _myCommonPlc3D->createSegment(1,3);
//    _myCommonPlc3D->createSegment(2,3);
//    _myCommonPlc3D->createSegment(4,5);
//    _myCommonPlc3D->createSegment(4,6);
//    _myCommonPlc3D->createSegment(5,7);
//    _myCommonPlc3D->createSegment(6,7);
//    _myCommonPlc3D->createSegment(0,4);
//    _myCommonPlc3D->createSegment(1,5);
//    _myCommonPlc3D->createSegment(2,6);
//    _myCommonPlc3D->createSegment(3,7);

//    {int _n[]={0,1,2}; _myCommonPlc3D->createFacet(_n);}
//    {int _n[]={5,6,7}; _myCommonPlc3D->createFacet(_n);}

    DelaunayGridGenerator::DelaunayGridGenerator3D *_myGenerator3D =
            new DelaunayGridGenerator::DelaunayGridGenerator3D();
    //_myGenerator3D->_TEST_constructGrid(_myCommonPlc3D);

    SimpleGLRender _render(NULL);
    _render.setRenderingPiecewiseLinearComplex(_myCommonPlc3D);
    _render.setRenderingDelaunayGridGenerator3D(_myGenerator3D);
    _render.ScenePos(
                (_myCommonPlc3D->getMaxCoord(0)-_myCommonPlc3D->getMinCoord(0))/2,
                (_myCommonPlc3D->getMaxCoord(1)-_myCommonPlc3D->getMinCoord(1))/2,
                (_myCommonPlc3D->getMaxCoord(2)-_myCommonPlc3D->getMinCoord(2))/2);
    _render.initializeGL();
    _render.setWindowTitle("Triangulation");
    //_render.move(0,0);
    _render.resize(800,600);
    _render.show();

    return app.exec();
}


