#include "uiwebmainwidget.h"
#include "version.h"
UiWebMainWidget::UiWebMainWidget(const WEnvironment &env):
    WApplication(env)
{
    QString _title = QString("PECAD:rev") + QString::number(VERSION_BUILD);
    this->setTitle(_title.toStdWString());
}

UiWebMainWidget::~UiWebMainWidget()
{
}
