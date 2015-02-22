#ifndef TEST_MATHUTILS_H
#define TEST_MATHUTILS_H

#include <QTest>
#include "mathutils.h"

class Test_MathUtils : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_factorial();
    private: Q_SLOT void test_round();
    private: Q_SLOT void test_trunc();
    private: Q_SLOT void test_simpleMatrix();
    private: Q_SLOT void test_calculateCircumSphereCenter();
    private: Q_SLOT void test_calculateCircumSphereCenterByCayleyMengerDeterminant();
    private: Q_SLOT void test_calculateIsNotDelaunayStatus();
    private: Q_SLOT void test_calculateIsSamePlaneStatus();
    private: Q_SLOT void test_calculateIsSamePlaneStatusByMatrixRank();
    private: Q_SLOT void test_calculateGeneralizedCrossProduct();
    private: Q_SLOT void test_calculateSimplexVoulumeByCayleyMengerDeterminant();
    private: Q_SLOT void test_calculateBarycentricCoordinates();
    private: Q_SLOT void test_calculateSegmentSubsimplexBarycenticIntersection();
    private: Q_SLOT void test_calculateSubsimplexSubsimplexIntersectionTrunc();
};

#endif // TEST_MATHUTILS_H
