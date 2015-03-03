#include "test_finiteelement.h"

#include <iostream>

using namespace MathUtils;

void Test_FiniteElement::test()
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
        FiniteElement<Node3D,4,3> element(&p, indexes);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
#endif

    int indexes[] = {0,1,2,3};
    FiniteElement<Node3D,4,3> element1(&p, indexes);
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
    FiniteElement<Node3D,4,3> element2(element1);
    element2[0][0] = 20;
    QVERIFY((*p[0])[0] == 20 && element1[0][0] == 20);
#ifdef _DEBUG_MODE
    try
    {
        indexes[1] = 999;
        FiniteElement<Node3D,4,3> element(&p, indexes);
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
}
