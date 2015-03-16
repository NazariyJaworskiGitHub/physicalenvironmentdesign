#ifndef TEST_CLMANAGER_H
#define TEST_CLMANAGER_H

#include "CLMANAGER/clmanager.h"
#include <QTest>

class Test_CLManager : public QObject
{
    Q_OBJECT
    private: Q_SLOT void testHelloWorld();
};

#endif // TEST_CLMANAGER_H
