/// \file \author Nazariy Jaworski

#include "test_mainwidget.h"

#include "UnitTests/dummy_wserver.h"
#include <Wt/Test/WTestEnvironment>

void Test_MainWidget::test()
{
    Test::WTestEnvironment _environment(".","wtconfig.xml");
    MainWidget *_mw = new MainWidget(_environment, nullptr);
    /// \todo deploy all widgets
    delete _mw;
}
