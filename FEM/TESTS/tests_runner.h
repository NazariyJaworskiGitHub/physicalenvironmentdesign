#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

#include "TESTS/test_finiteelement.h"
#include "TESTS/test_node.h"

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

void run_tests_Node()
{
    Test_Node _myTest_Node;
    QTest::qExec(&_myTest_Node, arguments);
}

void run_tests_FiniteElement()
{
    Test_FiniteElement _myTest_FiniteElement;
    QTest::qExec(&_myTest_FiniteElement, arguments);
}

void run_tests_all()
{
    run_tests_Node();
    run_tests_FiniteElement();
}
#endif // TESTS_RUNNER_H
