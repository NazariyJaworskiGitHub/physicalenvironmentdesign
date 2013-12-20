/// \file \author Nazariy Jaworski

#ifndef TEST_MAINWIDGET_H
#define TEST_MAINWIDGET_H

#include <QTest>
#include "Ui/Web/mainwidget.h"

using namespace Ui::Web;

class Test_MainWidget: public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_MAINWIDGET_H
