#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <QTest>
#include "generator.h"

class Test_Generator : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_1();
    private: Q_SLOT void test_2();
    private: Q_SLOT void test_3();
    private: Q_SLOT void test_4();
};

#endif // TEST_GENERATOR_H
