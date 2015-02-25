#ifndef TEST_GEOMETRICOBJECTS_H
#define TEST_GEOMETRICOBJECTS_H

#include <QTest>
#include "geometricobjects.h"

class Test_GeometricObjects : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_Icosahedron_creation();
};

#endif // TEST_GEOMETRICOBJECTS_H
