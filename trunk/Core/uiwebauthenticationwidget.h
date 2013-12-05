/// \author Nazariy Jaworski

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

        /// Should point to UserSession* of top-level widget, see UiWebAuthenticationWidget()
        /// if you call delete, dont forget to set it with nullptr
    private: UserSession    ** const _myUserSession;

    private: bool   _isLogInState;
    public : WLabel         *myUserNameLabel        = nullptr;
    public : WLabel         *myUserPassWordLabel    = nullptr;
    public : WLineEdit      *myUserNameLineEdit     = nullptr;
    public : WLineEdit      *myPassWordLineEdit     = nullptr;
    public : WPushButton    *myLogInOutButton       = nullptr;
    public : WLabel         *myInfoMessageLabel     = nullptr;

        /// Common constructor
        /// qObjParent currently not used
    public : UiWebAuthenticationWidget(
            UserSession ** const ptrToUserSession,
            QObject *qObjParent,
            WContainerWidget *wContParent);
        /// Slot for myLogInOutButton
    public : void onLogInOutButton();
        /// Change widget to display log in fields
    public : /*Q_SLOT*/ void changeToLogInState();
        /// Change widget to display log out fields
    public : /*Q_SLOT*/ void changeToLogOutState();
        /// Common destructor
    public : ~UiWebAuthenticationWidget();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;

    /*public : Q_SIGNAL void createUserSession(Guard::UserData *ptrToUserData) const;
        ///< sends signal to UiWebMainWidget to start the new UserSession
    public : Q_SIGNAL void destroyUserSession() const;
        ///< sends signal to UiWebMainWidget to destroy the UserSession

    public : Signal<Guard::UserData *> createUserSession_s;
    public : Signal<> destroyUserSession_s;*/
};

#endif // UIWEBAUTHENTICATIONWIDGET_H
