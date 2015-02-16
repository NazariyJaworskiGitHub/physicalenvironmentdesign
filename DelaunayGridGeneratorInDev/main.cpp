#include <simpleglrender2d.h>
#include <simpleglrender3d.h>

#include "TESTS/tests_runner.h"

int main(int argc, char *argv[])
{
    run_tests_all();

    QApplication app(argc, argv);

/*    DelaunayGridGenerator::Plc3D *_myPlc3D =
            new DelaunayGridGenerator::Plc3D();

    /// TEST 5
    for(int i=0; i<100; i++)
    {
        _myPlc3D->createNode(
                    FEM::Node3D(
                        MathUtils::rand(0.0,1.0),
                        MathUtils::rand(0.0,1.0),
                        MathUtils::rand(0.0,1.0)));
    }

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

    SimpleGLRender3D _render(NULL);
    _render.setRenderingPiecewiseLinearComplex(_myPlc3D);
    _render.setRenderingDelaunayGridGenerator3D(_myGenerator3D);
    _render.setScenePosition(FEM::Node3D(
                (_myPlc3D->getMaxCoord(0)-_myPlc3D->getMinCoord(0))/2,
                (_myPlc3D->getMaxCoord(1)-_myPlc3D->getMinCoord(1))/2,
                (_myPlc3D->getMaxCoord(2)-_myPlc3D->getMinCoord(2))/2));
    _render.initializeGL();
    _render.setWindowTitle("Triangulation");
    _render.resize(800,600);
    _render.show();*/

    DelaunayGridGenerator::Plc2D _myPlc2D;

//    /// TEST 5
//    for(int i=0; i<100; i++)
//    {
//        _myPlc2D.createNode(FEM::Node2D(
//                                MathUtils::rand(0.0,1.0),
//                                MathUtils::rand(0.0,1.0)));
//    }

    /// TEST 6
    int N=20;
    for(int i=0; i<N; i++)
    {
        FEM::Node2D _n((1.0 + sin(i*2*M_PI/N))/2.0,
                    (1.0 + cos(i*2*M_PI/N))/2.0);
        _myPlc2D.createNode(_n);
    }

     _myPlc2D.updateMaxAndMinCoordinates();

    DelaunayGridGenerator::DelaunayGridGenerator2D _myGenerator2D;

    SimpleGLRender2D _render(NULL);
    _render.setRenderingPiecewiseLinearComplex(&_myPlc2D);
    _render.setRenderingDelaunayGridGenerator2D(&_myGenerator2D);
    _render.setScenePosition(FEM::Node3D(
                (_myPlc2D.getMaxCoord(0)-_myPlc2D.getMinCoord(0))/2,
                (_myPlc2D.getMaxCoord(1)-_myPlc2D.getMinCoord(1))/2,
                0.0));
    _render.initializeGL();
    _render.setWindowTitle("Triangulation");
    _render.resize(800,600);
    _render.show();

    return app.exec();
}


