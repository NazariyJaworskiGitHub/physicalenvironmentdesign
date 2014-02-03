#include "test_finiteelement.h"

#include <iostream>
#include "node.h"

using namespace FEM;

void Test_FiniteElement::test()
{
    QList<Node3D> p = {{1,2,3}, {1,2,3}, {1,2,3}, {1,2,3}};

    try
    {
        int indexes[] = {0,1,2};
        FEM::SimplexElement<qreal,Node3D,3> element(&p, indexes);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }

    //deprecated FEM::FiniteElement<qreal,QVector3D,2,3> element1(&p, 0, 1);
    int indexes[] = {0,1,2,3};
    FEM::SimplexElement<qreal,Node3D,3> element1(&p, indexes);
    element1[0].setX(10);
    QVERIFY(p[0].x() == 10);

    try
    {
        element1[-2].setX(555);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    try
    {
        element1[999].setX(555);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    //deprecated  FEM::FiniteElement<qreal,QVector3D,2,3> element2(element1);
    FEM::SimplexElement<qreal,Node3D,3> element2(element1);
    element2[0].setX(20);
    QVERIFY(p[0].x() == 20 && element1[0].x() == 20);

    try
    {
        //deprecated  FEM::FiniteElement<qreal,QVector3D,2,3> element(&p, 999, 0.5);
        indexes[1] = 999;
        FEM::SimplexElement<qreal,Node3D,3> element(&p, indexes);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }

    // counterclockwise
    QList<FEM::Node2D> n = {{0,0.5}, {0,0}, {0.5,0}};
    qreal conductionCoefficients[3] = {100,100};
    //deprecated  FEM::FiniteElement<qreal,Node2D,3, 2> simplex(&n, 0, 1, 2);
    indexes[1] = 1;
    FEM::Triangle<qreal,Node2D> simplex(&n, indexes);
    Eigen::Matrix<qreal, 3, 3> K = simplex.calculateStiffnessMatrixEllipticEquation(conductionCoefficients);
    QVERIFY(    K(0,0)== 50 && K(0,1)==-50 && K(0,2)==  0 &&
                K(1,0)==-50 && K(1,1)==100 && K(1,2)==-50 &&
                K(2,0)==  0 && K(2,1)==-50 && K(2,2)== 50 );

    QList<Node3D> p2 = {{1,2,3}, {2,2,3}, {1,4,3}, {1,2,6}};
    int indexes2[] = {0,1,2,3};
    FEM::Tetrahedron<qreal,Node3D> thdr(&p2,indexes2);
    //QVERIFY( thdr.calculateSubElementVolume(0) == 6);
    QVERIFY( thdr.calculateSubElementVolume(1) == 6);
    QVERIFY( thdr.calculateSubElementVolume(2) == 3);
    QVERIFY( thdr.calculateSubElementVolume(3) == 2);
}
