#ifndef TEST_DERIVATIVE_H
#define TEST_DERIVATIVE_H

#include "FEM/derivative.h"
#include <QTest>

using namespace FEM;

class Test_Derivative : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_Derivative();
    private: Q_SLOT void test_DerivativeMapped();
    private: Q_SLOT void test_WeakOperator();
};

#endif // TEST_DERIVATIVE_H
