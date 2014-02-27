#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

void run_tests_DelaunayGridGenerator()
{
    /// \todo
    //Test_DelaunayGridGenerator _myTest_DelaunayGridGenerator;
    //QTest::qExec(&_myTest_DelaunayGridGenerator, arguments);
}

void run_tests_all()
{
    run_tests_DelaunayGridGenerator();
}
#endif // TESTS_RUNNER_H
