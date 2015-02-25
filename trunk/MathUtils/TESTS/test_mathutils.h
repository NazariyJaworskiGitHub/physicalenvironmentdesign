#ifndef TEST_MATHUTILS_H
#define TEST_MATHUTILS_H

#include <QTest>
#include "MathUtils"

class Test_MathUtils : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_factorial();
    private: Q_SLOT void test_round();
    private: Q_SLOT void test_trunc();
    private: Q_SLOT void test_simpleMatrix();
    private: Q_SLOT void test_calculateCircumSphereCenter();
    private: Q_SLOT void test_calculateCircumSphereCenterByCayleyMengerDeterminant();
    private: Q_SLOT void test_calculateIsCoplanarStatusWithClippingCheck();
    private: Q_SLOT void test_calculateIsSamePlaneStatusByMatrixRank();
    private: Q_SLOT void test_calculateGeneralizedCrossProduct();
    private: Q_SLOT void test_calculateSimplexVoulumeByCayleyMengerDeterminant();
    private: Q_SLOT void test_calculateBarycentricCoordinates();
    private: Q_SLOT void test_calculateSegmentSubsimplexBarycenticIntersection();
    private: Q_SLOT void test_calculateSubsimplexSubsimplexIntersection();
};

#endif // TEST_MATHUTILS_H
