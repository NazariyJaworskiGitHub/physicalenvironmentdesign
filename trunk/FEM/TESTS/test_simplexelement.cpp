#include "test_simplexelement.h"

#include <iostream>

using namespace FEM;
using namespace MathUtils;

void Test_SimplexElement::test()
{
    DefinedVectorType<Node3D*> p;
    p.push_back(new Node3D(1.0,2.0,3.0));
    p.push_back(new Node3D(1.0,2.0,3.0));
    p.push_back(new Node3D(1.0,2.0,3.0));
    p.push_back(new Node3D(1.0,2.0,3.0));

#ifdef _DEBUG_MODE
    try
    {
        int indexes[] = {0,1,2};
        FEM::SimplexElement<Node3D,3> element(&p, indexes);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
#endif

    int indexes[] = {0,1,2,3};
    FEM::SimplexElement<Node3D,3> element1(&p, indexes);
    element1[0][0] = 10;
    QVERIFY((*p[0])[0] == 10);

#ifdef _DEBUG_MODE
    try
    {
        element1[-2][0] = 555;
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    try
    {
        element1[999][0] = 555;
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
#endif
    FEM::SimplexElement<Node3D,3> element2(element1);
    element2[0][0] = 20;
    QVERIFY((*p[0])[0] == 20 && element1[0][0] == 20);

#ifdef _DEBUG_MODE
    try
    {
        indexes[1] = 999;
        FEM::SimplexElement<Node3D,3> element(&p, indexes);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
#endif
    delete(p[0]);
    delete(p[1]);
    delete(p[2]);
    delete(p[3]);
    p.clear();

    // Counterclockwise
    DefinedVectorType<Node2D*> n;
    n.push_back(new Node2D(0.0,0.5));
    n.push_back(new Node2D(0.0,0.0));
    n.push_back(new Node2D(0.5,0.0));
    MathUtils::Real conductionCoefficients[2] = {100,100};
    indexes[1] = 1;
    FEM::Triangle<Node2D> simplex(&n, indexes);
    Eigen::Matrix<MathUtils::Real, 3, 3> K =
            simplex.calculateStiffnessMatrixEllipticEquation(conductionCoefficients);
    QVERIFY(    K(0,0)== 50 && K(0,1)==-50 && K(0,2)==  0 &&
                K(1,0)==-50 && K(1,1)==100 && K(1,2)==-50 &&
                K(2,0)==  0 && K(2,1)==-50 && K(2,2)== 50 );
    delete(n[0]);
    delete(n[1]);
    delete(n[2]);
    n.clear();

    DefinedVectorType<Node3D*> p2;
    p2.push_back(new Node3D(1.0,2.0,3.0));
    p2.push_back(new Node3D(2.0,2.0,3.0));
    p2.push_back(new Node3D(1.0,4.0,3.0));
    p2.push_back(new Node3D(1.0,2.0,6.0));
    int indexes2[] = {0,1,2,3};
    FEM::Tetrahedron<Node3D> thdr(&p2,indexes2);
    //QVERIFY( thdr.calculateSubElementVolume(0) == 6);
    QVERIFY( thdr.calculateSubElementVolume(1) == 3.0);
    QVERIFY( thdr.calculateSubElementVolume(2) == 1.5);
    QVERIFY( thdr.calculateSubElementVolume(3) == 1.0);

    delete(p2[0]);
    delete(p2[1]);
    delete(p2[2]);
    delete(p2[3]);
    p2.clear();
}
