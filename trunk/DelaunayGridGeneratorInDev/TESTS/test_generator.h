#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <QTest>
#include "generator.h"

class Test_Generator : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_GENERATOR_H
