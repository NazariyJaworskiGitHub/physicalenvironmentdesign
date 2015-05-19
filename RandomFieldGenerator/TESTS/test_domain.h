#ifndef TEST_DOMAIN_H
#define TEST_DOMAIN_H

#include "FEM/domain.h"
#include <QTest>

using namespace FEM;

class Test_Domain : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_RVEDomain();
};

#endif // TEST_DOMAIN_H
