#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

#include "TESTS/test_mathutils.h"
#include "TESTS/test_node.h"
#include "TESTS/test_precision.h"
#include "TESTS/test_extendedreal.h"

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

//void run_tests_Precision()
//{
//    Test_Precision _myTest_Precision;
//    QTest::qExec(&_myTest_Precision, arguments);
//}

void run_tests_Node()
{
    Test_Node _myTest_Node;
    QTest::qExec(&_myTest_Node, arguments);
}

void run_tests_ExtendedReal()
{
    Test_ExtendedReal _myTest_ExtendedReal;
    QTest::qExec(&_myTest_ExtendedReal, arguments);
}

void run_tests_MathUtils()
{
    Test_MathUtils _myTest_MathUtils;
    QTest::qExec(&_myTest_MathUtils, arguments);
}

void run_tests_all()
{
//    run_tests_Precision();
    run_tests_Node();
    run_tests_ExtendedReal();
    run_tests_MathUtils();
}
#endif // TESTS_RUNNER_H
