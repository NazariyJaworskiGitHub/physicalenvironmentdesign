#ifndef UIWEBMAINWIDGET_H
#define UIWEBMAINWIDGET_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <QObject>
#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "uiwebauthenticationwidget.h"

using namespace Wt;

/// main widget for web users
class UiWebMainWidget : public QObject, public WApplication
{
    Q_OBJECT

    public : WVBoxLayout *myVBoxLayout = nullptr;
        ///< top-level vertical box layout
    public :UiWebAuthenticationWidget *myUiWebAuthenticationWidget = nullptr;
        ///< Authentication widget
    public : UiWebMainWidget(const WEnvironment &env);
        ///< Common constructor
    public : ~UiWebMainWidget();
        ///< common destructor
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // UIWEBMAINWIDGET_H

