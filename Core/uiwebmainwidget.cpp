#include "uiwebmainwidget.h"
#include "version.h"
#include "core.h"

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

    /// \todo put here some layout

    myUiWebAuthenticationWidget = new UiWebAuthenticationWidget(root());
    //myUiWebAuthenticationWidget->setHeight(WLength(40));    ///<\todo magic number!!!
    connect(myUiWebAuthenticationWidget, SIGNAL(writeString(QString)), Core::instance()->myLogger, SLOT(writeToLog(QString)));
    root()->addWidget(myUiWebAuthenticationWidget);
    root()->addWidget(new WBreak());
}

UiWebMainWidget::~UiWebMainWidget()
{
    if(myUiWebAuthenticationWidget)
        delete myUiWebAuthenticationWidget;
}
