#ifndef TEST_NODE_H
#define TEST_NODE_H

#include <QTest>
#include "MathUtils"

class Test_Node : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_NODE_H
