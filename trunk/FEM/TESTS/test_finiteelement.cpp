#include "test_finiteelement.h"

#include <iostream>
#include "node.h"

using namespace FEM;

void Test_FiniteElement::test()
{
    QVector3D p1(1,2,3);
    QVector3D p2(1,2,3);

    FEM::FiniteElement<qreal,QVector3D,2,3> element1(&p1, &p2);
    element1[0].setX(10);
    QVERIFY(p1.x() == 10);

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
    FEM::FiniteElement<qreal,QVector3D,2,3> element2(element1);
    element2[0].setX(20);
    QVERIFY(p1.x() == 20 && element1[0].x() == 20);

    // counterclockwise
    FEM::Node3D n1(0,0.5,0);
    FEM::Node3D n2(0,0,0);
    FEM::Node3D n3(0.5,0,0);
    FEM::FiniteElement<qreal,Node3D,3, 2> simplex(&n1, &n2, &n3);
    Eigen::Matrix<qreal, 2, 2> D;
    D << 100,   0,
         0,     100;
    Eigen::Matrix<qreal, 3, 3> K = simplex.calculateStiffnessMatrix(D);
    QVERIFY(    K(0,0)== 50 && K(0,1)==-50 && K(0,2)==  0 &&
                K(1,0)==-50 && K(1,1)==100 && K(1,2)==-50 &&
                K(2,0)==  0 && K(2,1)==-50 && K(2,2)== 50 );
}
