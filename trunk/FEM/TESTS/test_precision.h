#ifndef TEST_PRECISION_H
#define TEST_PRECISION_H

#include <QTest>
#include "mathutils.h"

/// For testing the MathUtils and their precision robustness
class Test_Precision : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_PRECISION_H
