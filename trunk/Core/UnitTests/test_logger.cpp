/// \file \author Nazariy Jaworski

#include "test_logger.h"

void Test_Logger::test()
{
    Logger *_l1 = new Logger();
    _l1->openLogFile();
    delete _l1;

    _l1 = new Logger(nullptr);
    _l1->openLogFile("test_LogFile1");
    _l1->openLogFile("test_LogFile2");
    _l1->openLogFile("test_LogFile2");
    _l1->openLogFile("test_LogFile1");
    delete _l1;

    QFile* _test_LogFile1 = new QFile("test_LogFile1");
    QFile* _test_LogFile2 = new QFile("test_LogFile2");
    qint64 _size1 = _test_LogFile1->size();
    qint64 _size2 = _test_LogFile2->size();

    _l1 = new Logger(nullptr);
    _l1->openLogFile("test_LogFile1");
    Logger *_l2 = new Logger(nullptr);
    _l2->openLogFile("test_LogFile2");
    _l1->writeToLog("test_data1\n");
    _l2->writeToLog("test_data2\n");

    QVERIFY(_size1 < _test_LogFile1->size());
    QVERIFY(_size2 < _test_LogFile2->size());

    delete _l1;
    delete _l2;
    delete _test_LogFile1;
    delete _test_LogFile2;
}
