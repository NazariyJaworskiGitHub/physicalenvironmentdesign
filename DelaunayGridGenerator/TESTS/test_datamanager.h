#ifndef TEST_DATAMANAGER_H
#define TEST_DATAMANAGER_H

#include <QTest>
#include "datamanager.h"

class Test_DataManager : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_DATAMANAGER_H
