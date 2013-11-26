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
// |Contact information, license,etc.                   |
// +----------------------------------------------------+

UiWebMainWidget::UiWebMainWidget(const WEnvironment &env):
    WApplication(env)
{
    QString _title = QString("PECAD:rev") + QString::number(VERSION_BUILD);
    this->setTitle(_title.toStdWString());

    /// \todo put here some layout

    myUiWebAuthenticationWidget = new UiWebAuthenticationWidget(
                myUserSession, root());
    connect(myUiWebAuthenticationWidget, SIGNAL(writeString(QString)),
            Core::instance()->myLogger, SLOT(writeToLog(QString)));
    root()->addWidget(myUiWebAuthenticationWidget);
    root()->addWidget(new WBreak());
}

UiWebMainWidget::~UiWebMainWidget()
{
    if(myUiWebAuthenticationWidget)
        delete myUiWebAuthenticationWidget;
}
