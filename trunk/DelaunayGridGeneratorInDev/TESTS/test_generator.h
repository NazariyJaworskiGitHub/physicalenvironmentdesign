#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <QTest>
#include "generator.h"

class Test_Generator : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_1(); // Bad PLC test
    private: Q_SLOT void test_2(); // Element creation test
    private: Q_SLOT void test_3(); // Delaunay criteria node order test
    private: Q_SLOT void test_4(); // Inner lists update test
    private: Q_SLOT void test_5(); // Intersections test
};

#endif // TEST_GENERATOR_H
