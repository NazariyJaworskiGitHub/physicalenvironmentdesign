/// \file \author Nazariy Jaworski

#ifndef UNIT_TESTS_RUNNER_H
#define UNIT_TESTS_RUNNER_H

#include <QStringList>

#include "test_core.h"
#include "test_guard.h"
#include "test_logger.h"
#include "test_mainwidget.h"
#include "test_uiwebserver.h"

//tip! use "-vs" to see emited signals

void run_tests_Core()
{
    Test_Core _myTest_Core;
    QStringList arguments = {
    //    " ","-o","UnitTestsResults/UnitTest_Core.txt"};
        " ","-silent"};
    QTest::qExec(&_myTest_Core, arguments);
}
void run_tests_Logger()
{
    Test_Logger _myTest_Logger;
    QStringList arguments = {
    //    " ","-o","UnitTestsResults/UnitTest_Logger.txt"};
        " ","-silent"};
    QTest::qExec(&_myTest_Logger, arguments);
}
void run_tests_Guard()
{
    Test_Guard _myTest_Guard;
    QStringList arguments = {
    //    " ","-o","UnitTestsResults/UnitTest_Logger.txt"};
        " ","-silent"};
    QTest::qExec(&_myTest_Guard, arguments);
}
void run_tests_UiWebServer()
{
    Test_UiWebServer _myTest_UiWebServer;
    QStringList arguments = {
    //    " ","-o","UnitTestsResults/UnitTest_Logger.txt"};
        " ","-silent"};
    QTest::qExec(&_myTest_UiWebServer, arguments);
}
void run_tests_MainWidget()
{
    Test_MainWidget _myTest_MainWidget;
    QStringList arguments = {
    //    " ","-o","UnitTestsResults/UnitTest_Logger.txt"};
        " ","-silent"};
    QTest::qExec(&_myTest_MainWidget, arguments);
}
void run_tests_all()
{
    //QDir qtestLogDir;
    //qtestLogDir.mkdir("UnitTestsResults");
    run_tests_Core();
    run_tests_Logger();
    run_tests_Guard();
    run_tests_UiWebServer();
    run_tests_MainWidget();
}
#endif // UNIT_TESTS_RUNNER_H
