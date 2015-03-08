#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

#include "test_clmanager.h"

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

void run_tests_CLManager()
{
    Test_CLManager _myTest_CLManager;
    QTest::qExec(&_myTest_CLManager, arguments);
}

void run_tests_all()
{
    run_tests_CLManager();
}
#endif // TESTS_RUNNER_H
