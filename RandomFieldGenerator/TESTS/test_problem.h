#ifndef TEST_PROBLEM_H
#define TEST_PROBLEM_H

#include "FEM/problem.h"
#include <QTest>

using namespace FEM;

class Test_Problem : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_HeatConduction_constructLocalStiffnessMatrix();
    private: Q_SLOT void test_Elasticity_constructLocalStiffnessMatrix();
};

#endif // TEST_PROBLEM_H
