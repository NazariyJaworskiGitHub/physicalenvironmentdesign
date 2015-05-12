#ifndef TEST_SIMPLEMATRIX_H
#define TEST_SIMPLEMATRIX_H

#include "matrix.h"
#include <QTest>

using namespace MathUtils;

class Test_Matrix : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_Matrix();
};

#endif // TEST_SIMPLEMATRIX_H
