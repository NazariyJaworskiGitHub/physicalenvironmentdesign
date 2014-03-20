#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

#include "TESTS/test_beam.h"
#include "TESTS/test_finiteelement.h"
#include "TESTS/test_node.h"
#include "TESTS/test_precision.h"

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

void run_tests_Precision()
{
    Test_Precision _myTest_Precision;
    QTest::qExec(&_myTest_Precision, arguments);
}

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

void run_tests_Beam()
{
    Test_Beam _myTest_Beam;
    QTest::qExec(&_myTest_Beam, arguments);
}

void run_tests_all()
{
    run_tests_Precision();
    run_tests_Node();
    run_tests_FiniteElement();
    run_tests_Beam();
}
#endif // TESTS_RUNNER_H
