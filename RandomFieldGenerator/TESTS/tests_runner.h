#ifndef TESTS_RUNNER_H
#define TESTS_RUNNER_H

#include <QStringList>

#include "test_clmanager.h"
#include "test_viennacl.h"
#include "test_polynomial.h"
#include "test_derivative.h"
#include "test_matrix.h"
#include "test_fespacesimplex.h"
#include "test_problem.h"
#include "test_simulation.h"

//tip! use "-vs" to see emited signals
QStringList arguments = {
//    " ","-o","UnitTestsResults/UnitTest_XXX.txt"};
    " ","-silent"};

void run_tests_CLManager()
{
    Test_CLManager _myTest_CLManager;
    QTest::qExec(&_myTest_CLManager, arguments);
}
void run_tests_ViennaCL()
{
    Test_ViennaCL _myTest_ViennaCL;
    QTest::qExec(&_myTest_ViennaCL, arguments);
}
void run_tests_Polynomial()
{
    Test_Polynomial _myTest_Polynomial;
    QTest::qExec(&_myTest_Polynomial, arguments);
}
void run_tests_Derivative()
{
    Test_Derivative _myTest_Derivative;
    QTest::qExec(&_myTest_Derivative, arguments);
}
void run_tests_Matrix()
{
    Test_Matrix _myTest_Matrix;
    QTest::qExec(&_myTest_Matrix, arguments);
}
void run_tests_FESpaceSimplex()
{
    Test_FESpaceSimplex _myTest_FESpaceSimplex;
    QTest::qExec(&_myTest_FESpaceSimplex, arguments);
}
void run_tests_Problem()
{
    Test_Problem _myTest_Problem;
    QTest::qExec(&_myTest_Problem, arguments);
}
void run_tests_Simulation()
{
    Test_Simulation _myTest_Simulation;
    QTest::qExec(&_myTest_Simulation, arguments);
}
void run_tests_all()
{
    run_tests_CLManager();
    run_tests_ViennaCL();
    run_tests_Polynomial();
    run_tests_Derivative();
    run_tests_Matrix();
    run_tests_FESpaceSimplex();
    run_tests_Problem();
    run_tests_Simulation();
}
#endif // TESTS_RUNNER_H
