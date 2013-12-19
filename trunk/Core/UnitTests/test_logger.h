/// \file \author Nazariy Jaworski

#ifndef TEST_LOGGER_H
#define TEST_LOGGER_H

#include <QTest>
#include <logger.h>
class Test_Logger : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_LOGGER_H
