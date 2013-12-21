/// \file \author Nazariy Jaworski

#ifndef TEST_USERSESSION_H
#define TEST_USERSESSION_H

#include "Session/usersession.h"

#include <QTest>
#include "logger.h"
class Test_UserSession : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_USERSESSION_H
