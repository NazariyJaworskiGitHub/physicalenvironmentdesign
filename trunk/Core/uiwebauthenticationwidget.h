#ifndef UIWEBAUTHENTICATIONWIDGET_H
#define UIWEBAUTHENTICATIONWIDGET_H

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
#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WLabel>
#include <Wt/WLineEdit>

#include "usersession.h"

using namespace Wt;

/// Implements Logging In and Out \n
/// It is simle avoiding of common Wt or Qt Authentication classes
// tip! QObject should always be the first
class UiWebAuthenticationWidget : public QObject, public WContainerWidget
{
    Q_OBJECT

    private: UserSession* _myUserSession;
        ///< should point to UserSession* of top-level widget, see UiWebAuthenticationWidget()
    private: bool   _isLogInState;
    public : WLabel         *myUserNameLabel        = nullptr;
    public : WLabel         *myUserPassWordLabel    = nullptr;
    public : WLineEdit      *myUserNameLineEdit     = nullptr;
    public : WLineEdit      *myPassWordLineEdit     = nullptr;
    public : WPushButton    *myLogInOutButton       = nullptr;
    public : WLabel         *myInfoMessageLabel     = nullptr;
    public : UiWebAuthenticationWidget(
            UserSession* ptrToUserSession,
            WContainerWidget *parent);
        ///< Common constructor
    public : void onLogInOutButton();
        ///< slot for myLogInOutButton
    private: void _changeToLogInState();
        ///< change widget to display log in fields
    private: void _changeToLogOutState();
        ///< change widget to display log out fields
    public : ~UiWebAuthenticationWidget();
        ///< Common destructor
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // UIWEBAUTHENTICATIONWIDGET_H
