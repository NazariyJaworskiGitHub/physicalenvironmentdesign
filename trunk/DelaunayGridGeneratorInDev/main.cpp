#include <simpleglrender.h>

#include "TESTS/tests_runner.h"

int main(int argc, char *argv[])
{
    run_tests_all();

    QApplication app(argc, argv);

    DelaunayGridGenerator::Plc3D *_myPlc3D =
            new DelaunayGridGenerator::Plc3D();

    /// TEST 2
    _myPlc3D->createNode(FEM::Node3D(0.0,0.0,0.0));
    _myPlc3D->createNode(FEM::Node3D(1.0,0.0,0.0));
    _myPlc3D->createNode(FEM::Node3D(0.0,1.0,0.0));
    _myPlc3D->createNode(FEM::Node3D(1.0,1.0,0.0));
    _myPlc3D->createNode(FEM::Node3D(0.0,0.0,1.0));
    _myPlc3D->createNode(FEM::Node3D(1.0,0.0,1.0));
    _myPlc3D->createNode(FEM::Node3D(0.0,1.0,1.0));
    _myPlc3D->createNode(FEM::Node3D(1.0,1.0,1.0));

//    /// TEST 4
//    for(int i=0; i<50; i++)
//    {
//        _myPlc3D->createNode(
//                    FEM::Node3D(
//                        MathUtils::rand(0.0,1.0),
//                        MathUtils::rand(0.0,1.0),
//                        MathUtils::rand(0.0,1.0)));
//    }

    _myPlc3D->updateMaxAndMinCoordinates();

//    _myPlc3D->createSegment(0,1);
//    _myPlc3D->createSegment(0,2);
//    _myPlc3D->createSegment(1,3);
//    _myPlc3D->createSegment(2,3);
//    _myPlc3D->createSegment(4,5);
//    _myPlc3D->createSegment(4,6);
//    _myPlc3D->createSegment(5,7);
//    _myPlc3D->createSegment(6,7);
//    _myPlc3D->createSegment(0,4);
//    _myPlc3D->createSegment(1,5);
//    _myPlc3D->createSegment(2,6);
//    _myPlc3D->createSegment(3,7);

//    {int _n[]={0,1,2}; _myPlc3D->createFacet(_n);}
//    {int _n[]={5,6,7}; _myPlc3D->createFacet(_n);}

    DelaunayGridGenerator::DelaunayGridGenerator3D *_myGenerator3D =
            new DelaunayGridGenerator::DelaunayGridGenerator3D();

    SimpleGLRender _render(NULL);
    _render.setRenderingPiecewiseLinearComplex(_myPlc3D);
    _render.setRenderingDelaunayGridGenerator3D(_myGenerator3D);
    _render.ScenePos(
                (_myPlc3D->getMaxCoord(0)-_myPlc3D->getMinCoord(0))/2,
                (_myPlc3D->getMaxCoord(1)-_myPlc3D->getMinCoord(1))/2,
                (_myPlc3D->getMaxCoord(2)-_myPlc3D->getMinCoord(2))/2);
    _render.initializeGL();
    _render.setWindowTitle("Triangulation");
    _render.resize(800,600);
    _render.show();

    return app.exec();
}


