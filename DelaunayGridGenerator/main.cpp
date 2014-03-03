#include <iostream>
#include <conio.h>

#include "TESTS/tests_runner.h"
#include "UTILS/utilwebserver.h"

#include "piecewiselinearcomplex.h"

int main()
{
    run_tests_all();

    /*DelaunayGridGenerator::CommonPlc3D _myCommonPlc3D;

    _myCommonPlc3D.createNode(FEM::Node3D(0,0,0));
    _myCommonPlc3D.createNode(FEM::Node3D(1,0,0));
    _myCommonPlc3D.createNode(FEM::Node3D(0,1,0));
    _myCommonPlc3D.createNode(FEM::Node3D(1,1,0));
    _myCommonPlc3D.createNode(FEM::Node3D(0,0,1));
    _myCommonPlc3D.createNode(FEM::Node3D(1,0,1));
    _myCommonPlc3D.createNode(FEM::Node3D(0,1,1));
    _myCommonPlc3D.createNode(FEM::Node3D(1,1,1));

    _myCommonPlc3D.createSegment(0,1);
    _myCommonPlc3D.createSegment(0,2);
    _myCommonPlc3D.createSegment(1,3);
    _myCommonPlc3D.createSegment(2,3);
    _myCommonPlc3D.createSegment(4,5);
    _myCommonPlc3D.createSegment(4,6);
    _myCommonPlc3D.createSegment(5,1);
    _myCommonPlc3D.createSegment(6,7);

    {int _n[]={0,1,2}; _myCommonPlc3D.createFacet(_n);}
    {int _n[]={5,6,7}; _myCommonPlc3D.createFacet(_n);}*/

    Utilities::WebServer *_myWebServer = new Utilities::WebServer();
    _myWebServer->startServer();    // it goes in different thread

    getch();

    return 0;
}


