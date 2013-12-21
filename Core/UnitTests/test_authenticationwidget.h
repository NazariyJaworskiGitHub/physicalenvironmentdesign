/// \file \author Nazariy Jaworski

#ifndef TEST_AUTHENTICATIONWIDGET_H
#define TEST_AUTHENTICATIONWIDGET_H

#include <QTest>
#include "Ui/Web/authenticationwidget.h"

using namespace Ui::Web;

class Test_AuthenticationWidget : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_AUTHENTICATIONWIDGET_H
