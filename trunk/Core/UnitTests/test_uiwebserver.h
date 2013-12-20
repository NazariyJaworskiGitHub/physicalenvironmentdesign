/// \file \author Nazariy Jaworski

#ifndef TEST_UIWEBSERVER_H
#define TEST_UIWEBSERVER_H

#include <QTest>
#include "Ui/Web/uiwebserver.h"

using namespace Ui::Web;

class Test_UiWebServer: public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_UIWEBSERVER_H
