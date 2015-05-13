#ifndef TEST_POLYNOMIAL_H
#define TEST_POLYNOMIAL_H

#include "FEM/polynomial.h"
#include <QTest>

using namespace FEM;

class Test_Polynomial : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_Polynomial();
};

#endif // TEST_POLYNOMIAL_H
