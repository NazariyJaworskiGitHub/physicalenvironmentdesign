/// \file \author Nazariy Jaworski

#ifndef TEST_CORE_H
#define TEST_CORE_H

#include <QTest>
#include <core.h>
class Test_Core : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_CORE_H
