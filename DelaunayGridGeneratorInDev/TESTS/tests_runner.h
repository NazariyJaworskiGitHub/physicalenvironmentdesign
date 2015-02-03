#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

#include "test_piecewiselinearcomplex.h"

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

void run_tests_PiecewiseLinearComplex()
{
    Test_PiecewiseLinearComplex _myTest_PiecewiseLinearComplex;
    QTest::qExec(&_myTest_PiecewiseLinearComplex, arguments);
}

void run_tests_all()
{
    run_tests_PiecewiseLinearComplex();
}
#endif // TESTS_RUNNER_H
