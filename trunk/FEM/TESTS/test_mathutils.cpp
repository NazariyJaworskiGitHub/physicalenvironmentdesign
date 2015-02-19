#include "test_mathutils.h"

#include <iostream>

#include "node.h"

using namespace MathUtils;
using namespace FEM;

void Test_MathUtils::test_factorial()
{
    QVERIFY(factorial(0) == 1);
    QVERIFY(factorial(1) == 1);
    QVERIFY(factorial(2) == 2);
    QVERIFY(factorial(8) == 40320);
}

void Test_MathUtils::test_round()
{
    float _rez = MathUtils::round<float>(0.100025,1e-3);
    QVERIFY(_rez == 0.1f);
    _rez = MathUtils::round<float>(0.107,1e-2);
    QVERIFY(_rez == 0.11f);
    _rez = MathUtils::round<float>(-8.1e-8,1e-7);
    QVERIFY(_rez == -1e-7f);
}

void Test_MathUtils::test_trunc()
{
    float _rez = MathUtils::trunc<float>(0.100025,1e-3);
    QVERIFY(_rez == 0.1f);
    _rez = MathUtils::trunc<float>(0.107,1e-2);
    QVERIFY(_rez == 0.099999994f);
    _rez = MathUtils::trunc<float>(-8.1e-8,1e-7);
    QVERIFY(_rez == 0.0f);
}

void Test_MathUtils::test_calculateCircumSphereCenter()
{
    Node2D _simpleNodes2D[] = {{0,0},{1,0},{0,1}};
    Real _circumRadius1;

    Node2D _c1 = calculateCircumSphereCenter<Node2D,2>(
                _simpleNodes2D,&_circumRadius1);
    QVERIFY((_c1-Node2D(0.5,0.5))[0] < 1e-4);
    QVERIFY(std::fabs(_circumRadius1 - std::sqrt(2)/2) < 1e-4);
}

void Test_MathUtils::test_calculateCircumSphereCenterByCayleyMengerDeterminant()
{
    Node2D _simpleNodes2D[] = {{0,0},{1,0},{0,1}};
    Real _circumRadius1;
    Node2D _c1 = calculateCircumSphereCenter<Node2D,2>(
                _simpleNodes2D,&_circumRadius1);
    Real _circumRadius2;
    Real _circumRadius3;
    Node2D _c2 =
            calculateCircumSphereCenterByCayleyMengerDeterminant<Node2D,2>(
                _simpleNodes2D,3,&_circumRadius2);
    Node2D _c3 =
            calculateCircumSphereCenterByCayleyMengerDeterminant<Node2D,2>(
                _simpleNodes2D,2,&_circumRadius3);
    QVERIFY(_circumRadius1 == _circumRadius2);
    QVERIFY((_c1-_c2).isZero());
    QVERIFY((_c2-Node2D(0.5,0.5))[0] < 1e-4);
    QVERIFY(std::fabs(_circumRadius2 - std::sqrt(2)/2) < 1e-4);
    QVERIFY((_c3-Node2D(0.5,0.0))[0] < 1e-4);
    QVERIFY(std::fabs(_circumRadius3 - 0.5) < 1e-4);

    /// \todo 3D
}

void Test_MathUtils::test_calculateIsNotDelaunayStatus()
{
    /// \todo
}

void Test_MathUtils::test_calculateIsSamePlaneStatus()
{
    Node2D nodes[] = {{0.2,0.2}, {0.8,0.5}};
    Real _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(Node2D(0.6,0.2),nodes);
    QVERIFY(_result > 0);
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(Node2D(0.2,0.4),nodes);
    QVERIFY(_result < 0);
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(Node2D(0.4,0.3),nodes);
    QVERIFY(_result < 1e-4);
    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(Node2D(0.4,0.3),nodes);
    QVERIFY(_result < 1e-4);

    nodes[0]((1.0 + std::sin(0*2*M_PI/100.0))/2.0, (1.0 + std::cos(0*2*M_PI/100.0))/2.0);
    nodes[1]((1.0 + std::sin(1*2*M_PI/100.0))/2.0, (1.0 + std::cos(1*2*M_PI/100.0))/2.0);
    Node2D target1((1.0 + std::sin(2*2*M_PI/100.0))/2.0, (1.0 + std::cos(2*2*M_PI/100.0))/2.0);
    Node2D target2((1.0 + std::sin(3*2*M_PI/100.0))/2.0, (1.0 + std::cos(3*2*M_PI/100.0))/2.0);

    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(target1,nodes);
    QVERIFY(_result < 1e-4);
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(target2,nodes);
    QVERIFY(_result > 1e-4);

    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(target1,nodes);
    QVERIFY(_result > 1e-4);
    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(target2,nodes);
    QVERIFY(_result > 1e-4);

    nodes[0]((1.0 + std::sin(0*2*M_PI/1e6))/2.0, (std::cos(0*2*M_PI/1e6))/2.0);
    nodes[1]((1.0 + std::sin(1*2*M_PI/1e6))/2.0, (std::cos(1*2*M_PI/1e6))/2.0);
    target2 = nodes[0] - nodes[1];
    target1((1e6 + std::sin(2*2*M_PI/1e6))/2.0, (std::cos(2*2*M_PI/1e6))/2.0);
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(target1,nodes);
    QVERIFY(-_result < 1e-4);
    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(target1,nodes);
    QVERIFY((-_result < 1e-4 && target2[0] < 1e-4 && target2[1] < 1e-4) || (-_result > 1e-4));

    Real eps = std::numeric_limits<Real>::epsilon();
    nodes[0](MathUtils::rand<Real>(0.0, 1e3), MathUtils::rand<Real>(0.0, 1e3));
    nodes[1](MathUtils::rand<Real>(1e3, 0.0), MathUtils::rand<Real>(1e3, 0.0));
    target2 = MathUtils::rand<Real>( 0.0, 1e3) * (nodes[0] - nodes[1]) + nodes[1];
    target1 = MathUtils::rand<Real>(-1e3, 0.0) * (nodes[0] - nodes[1]) + nodes[1];
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(target1, nodes);
    QVERIFY(std::abs(_result) > eps);
    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(target1, nodes);
    QVERIFY(_result < eps);
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(target2, nodes);
    QVERIFY(std::abs(_result) > eps);
    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(target2, nodes);
    QVERIFY(_result < eps);
    target1[0] +=  (nodes[0][0] - nodes[1][0]) * (1.0 + 1 * eps);
    target1[1] += -(nodes[0][1] - nodes[1][1]) * (1.0 + 1 * eps);
    target2[0] -=  (nodes[0][0] - nodes[1][0]) * (1.0 + 1 * eps);
    target2[1] -= -(nodes[0][1] - nodes[1][1]) * (1.0 + 1 * eps);
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(target1, nodes);
    QVERIFY(std::abs(_result) > eps);
    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(target1, nodes);
    QVERIFY(std::abs(_result) > eps);
    _result = calculateIsCoplanarStatusWithClippingCheck<Node2D,2>(target2, nodes);
    QVERIFY(std::abs(_result) > eps);
    _result = calculateIsCoplanarStatusWithClippingCheckNormalized<Node2D,2>(target2, nodes);
    QVERIFY(std::abs(_result) > eps);
}

void Test_MathUtils::test_calculateIsSamePlaneStatusByMatrixRank()
{
    Node3D _simpleNodes3D[] = {{0,0,0}, {1,0,0}, {0,2,0}, {0,0,3}};
    bool _result;

    try
    {
        _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D,1);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D,2);
    QVERIFY(!_result);
    _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D,3);
    QVERIFY(!_result);
    _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D,4);
    QVERIFY(!_result);
    Node3D _simpleNodes3D_2[] = {{1,2,3}, {1,2,3}, {2,4,6}, {4,8,12}};
    try
    {
        _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D_2,0);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what() << '\n';
    }
    _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D_2,2);
    QVERIFY(_result);
    _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D_2,3);
    QVERIFY(_result);
    _result = calculateIsCoplanarStatusByMatrixRank<Node3D,3>(_simpleNodes3D_2,4);
    QVERIFY(_result);
}

void Test_MathUtils::test_calculateGeneralizedCrossProduct()
{
    Node2D nodes[] = {{0.2,0.2}, {0.8,0.5}};
    Node2D _normal = calculateGeneralizedCrossProduct<Node2D,2>(nodes);
    QVERIFY(_normal.fuzzyCompare(Node2D(0.3, -0.6),1e-4));

    Node3D nodes2[] = {{0.25, 1.15, -2.75}, {2.25, 6.15, 0.25}, {-0.75, 1.15, 1.25}};
    Node3D _normal2 = calculateGeneralizedCrossProduct<Node3D,3>(nodes2);
    QVERIFY(_normal2.fuzzyCompare(Node3D(20.0, -11.0, 5.0),1e-4));
}

void Test_MathUtils::test_calculateSimplexVoulumeByCayleyMengerDeterminant()
{
    Node2D nodes[] = {{0.2,0.2}, {0.2,0.8}, {0.8,0.5}};
    Real _result = calculateSimplexVoulumeByCayleyMengerDeterminant<Node2D>(nodes,3);
    QVERIFY( std::fabs(_result - 0.18) < 1e-4);
    Node3D nodes2[] = {{1.0,2.0,3.0}, {2.0,2.0,3.0}, {1.0,4.0,3.0}};
    _result = calculateSimplexVoulumeByCayleyMengerDeterminant<Node3D>(nodes2,3);
    QVERIFY( _result == 1.0);
    Node3D nodes3[] = {{1.0,2.0,3.0}, {1.0,4.0,3.0}, {1.0,2.0,6.0}};
    _result = calculateSimplexVoulumeByCayleyMengerDeterminant<Node3D>(nodes3,3);
    QVERIFY( _result == 3.0);
    Node3D nodes4[] = {{1.0,2.0,3.0}, {2.0,2.0,3.0}, {1.0,2.0,6.0}};
    _result = calculateSimplexVoulumeByCayleyMengerDeterminant<Node3D>(nodes4,3);
    QVERIFY( _result == 1.5);
}

void Test_MathUtils::test_calculateBarycentricCoordinates()
{
    Node2D _nodes[] = {{0.2,0.2}, {0.2,0.8}, {0.8,0.5}};
    Node3D _barycentricCoords = calculateBarycentricCoordinates<
            Node2D, Node3D, 2> (Node2D(0.5,0.5),_nodes);
    QVERIFY(_barycentricCoords.fuzzyCompare(Node3D(0.25,0.25,0.5),1e-4));
    _barycentricCoords = calculateBarycentricCoordinates<
            Node2D, Node3D, 2> (Node2D(0.9,0.5),_nodes);
    QVERIFY(_barycentricCoords.fuzzyCompare(Node3D(-0.08333,-0.08333,1.16667),1e-4));
    _barycentricCoords = calculateBarycentricCoordinates<
            Node2D, Node3D, 2> (Node2D(0.2,0.7),_nodes);
    QVERIFY(_barycentricCoords.fuzzyCompare(Node3D(0.16667,0.83333,0.0),1e-4));
}

void Test_MathUtils::test_calculateSegmentSubsimplexBarycenticIntersection()
{
    Node2D _nodes[] = {{0.2,0.2}, {0.8,0.5}};
    Node2D _intersection;
    bool _result = calculateSegmentSubsimplexBarycenticIntersection<Node2D, 2>(
                Node2D(0.6,0.2), Node2D(0.2,0.4), _nodes, &_intersection);
    QVERIFY(_result && _intersection.fuzzyCompare(Node2D(0.4,0.3)));
    _result = calculateSegmentSubsimplexBarycenticIntersection<Node2D, 2>(
                Node2D(0.4,0.2), Node2D(0.6,0.3), _nodes, &_intersection);
    QVERIFY(!_result);
    _result = calculateSegmentSubsimplexBarycenticIntersection<Node2D, 2>(
                Node2D(0.3,0.5), Node2D(0.5,0.1), _nodes, &_intersection);
    QVERIFY(_result && _intersection.fuzzyCompare(Node2D(0.4,0.3)));
    _result = calculateSegmentSubsimplexBarycenticIntersection<Node2D, 2>(
                Node2D(1.0,0.4), Node2D(0.9,0.7), _nodes, &_intersection);
    QVERIFY(!_result);
    _result = calculateSegmentSubsimplexBarycenticIntersection<Node2D, 2>(
                Node2D(0.8,0.0), Node2D(0.7,0.2), _nodes, &_intersection);
    QVERIFY(!_result);
    _result = calculateSegmentSubsimplexBarycenticIntersection<Node2D, 2>(
                Node2D(0.7,0.2), Node2D(0.8,0.0), _nodes, &_intersection);
    QVERIFY(!_result);
}

void Test_MathUtils::test_calculateSubsimplexSubsimplexIntersectionTrunc()
{
    Node2D _nodes2DA1[] = {{1.0,1.0}, {3.0,2.0}};
    Node2D _nodes2DB1[] = {{4.0,1.0}, {3.0,3.0}};
    bool _result = calculateSubsimplexSubsimplexIntersectionTrunc<Node2D, 2>(
                _nodes2DA1, _nodes2DB1);
    QVERIFY(!_result);
    _result = calculateSubsimplexSubsimplexIntersectionTruncNormalized<Node2D, 2>(
                _nodes2DA1, _nodes2DB1);
    QVERIFY(!_result);

    Node2D _nodes2DA2[] = {{1.0,1.0}, {3.0,2.0}};
    Node2D _nodes2DB2[] = {{4.0,1.0}, {3.0,2.0}};
    _result = calculateSubsimplexSubsimplexIntersectionTrunc<Node2D, 2>(
                _nodes2DA2, _nodes2DB2);
    QVERIFY(!_result);
    _result = calculateSubsimplexSubsimplexIntersectionTruncNormalized<Node2D, 2>(
                _nodes2DA2, _nodes2DB2);
    QVERIFY(!_result);

    Node2D _nodes2DA3[] = {{1.0,1.0}, {3.0,2.0}};
    Node2D _nodes2DB3[] = {{3.0,1.0}, {2.0,3.0}};
    _result = calculateSubsimplexSubsimplexIntersectionTrunc<Node2D, 2>(
                _nodes2DA3, _nodes2DB3);
    QVERIFY(_result);
    _result = calculateSubsimplexSubsimplexIntersectionTruncNormalized<Node2D, 2>(
                _nodes2DA3, _nodes2DB3);
    QVERIFY(_result);
}
