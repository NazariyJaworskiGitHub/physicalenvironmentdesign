#ifndef TEST_NODEWRAPPER_H
#define TEST_NODEWRAPPER_H

#include <QTest>
#include "nodewrapper.h"

class Test_NodeWrapper : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_NODEWRAPPER_H
