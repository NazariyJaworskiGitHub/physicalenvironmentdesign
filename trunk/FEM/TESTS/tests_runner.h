#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

#include "TESTS/test_beam.h"
#include "TESTS/test_simplexelement.h"

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

void run_tests_SimplexElement()
{
    Test_SimplexElement _myTest_SimplexElement;
    QTest::qExec(&_myTest_SimplexElement, arguments);
}

void run_tests_Beam()
{
    Test_Beam _myTest_Beam;
    QTest::qExec(&_myTest_Beam, arguments);
}

void run_tests_all()
{
    run_tests_SimplexElement();
    run_tests_Beam();
}
#endif // TESTS_RUNNER_H
