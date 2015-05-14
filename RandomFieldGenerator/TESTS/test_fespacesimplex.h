#ifndef TEST_FESPACESIMPLEX_H
#define TEST_FESPACESIMPLEX_H

#include "FEM/fespacesimplex.h"
#include <QTest>

using namespace FEM;

class Test_FESpaceSimplex : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_SimplexElement();
};

#endif // TEST_FESPACESIMPLEX_H
