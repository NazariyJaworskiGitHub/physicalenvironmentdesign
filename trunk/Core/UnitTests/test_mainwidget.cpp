/// \file \author Nazariy Jaworski

#include "test_mainwidget.h"

#include "UnitTests/dummy_wserver.h"
#include <Wt/Test/WTestEnvironment>

/// \todo connect Test::WTestEnvironment and libwttest
void Test_MainWidget::test()
{
    Test::WTestEnvironment environment(".","wtconfig.xml");
    MainWidget *_mw = new MainWidget(environment, nullptr);
    delete _mw;
}
