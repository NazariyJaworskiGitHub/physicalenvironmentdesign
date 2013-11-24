#include "uiwebmainwidget.h"
#include "version.h"
#include "core.h"

#include <Wt/WVBoxLayout>

// +----------------------------------------------------+
// | LogIn/LogOut widget                                |
// +----------------------------------------------------+
// | Working area                                       |
// |                                                    |
// |                                                    |
// |                                                    |
// |                                                    |
// |                                                    |
// |                                                    |
// +----------------------------------------------------+

UiWebMainWidget::UiWebMainWidget(const WEnvironment &env):
    WApplication(env)
{
    QString _title = QString("PECAD:rev") + QString::number(VERSION_BUILD);
    this->setTitle(_title.toStdWString());

    myVBoxLayout = new WVBoxLayout(root());
    root()->setLayout(myVBoxLayout);

    myUiWebAuthenticationWidget = new UiWebAuthenticationWidget(root());
    connect(myUiWebAuthenticationWidget, SIGNAL(writeString(QString)), Core::instance()->myLogger, SLOT(writeToLog(QString)));
    myVBoxLayout->addWidget(myUiWebAuthenticationWidget);
}

UiWebMainWidget::~UiWebMainWidget()
{
    if(myUiWebAuthenticationWidget)
        delete myUiWebAuthenticationWidget;
    if(myVBoxLayout)
        delete myVBoxLayout;
}
