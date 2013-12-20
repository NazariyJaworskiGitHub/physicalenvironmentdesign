/// \file \author Nazariy Jaworski

#ifndef TEST_GUARD_H
#define TEST_GUARD_H

#include <QTest>
#include "guard.h"
class Test_Guard : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_GUARD_H
