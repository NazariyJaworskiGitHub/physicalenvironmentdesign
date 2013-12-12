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

#include "Session/usersession.h"

using namespace Wt;

namespace Ui
{
    namespace Web
    {
        /// Implements Logging In and Out \n
        /// Creates and destroys specific UserSession
        /// It is simle avoiding of common Wt or Qt Authentication classes \n
        /// tip! QObject should always be the first
        class AuthenticationWidget : public QObject, public WContainerWidget
        {
            Q_OBJECT

                /// Should point to Session::UserSession* of top-level widget,
                /// see AuthenticationWidget()
                /// if you call delete, dont forget to set it with nullptr
            private: Session::UserSession ** const _myUserSession;

            private: bool           _isLogInState;
            public : WLabel         *myUserNameLabel        = nullptr;
            public : WLabel         *myUserPassWordLabel    = nullptr;
            public : WLineEdit      *myUserNameLineEdit     = nullptr;
            public : WLineEdit      *myPassWordLineEdit     = nullptr;
            public : WPushButton    *myLogInOutButton       = nullptr;
            public : WLabel         *myInfoMessageLabel     = nullptr;

                /// Common constructor
                /// qObjParent currently not used
            public : AuthenticationWidget(
                    Session::UserSession ** const ptrToUserSession,
                    QObject *qObjParent,
                    WContainerWidget *wContParent);
                /// Slot for myLogInOutButton
            public : void onLogInOutButton();
                /// Change widget to display log in fields
            public : void changeToLogInState();
                /// Change widget to display log out fields
            public : void changeToLogOutState();
                /// Common destructor
            public : ~AuthenticationWidget();
                /// Catch this signal with some Ui or Logger
            public : Q_SIGNAL void writeString(const QString message) const;
        };
    }
}

#endif // UIWEBAUTHENTICATIONWIDGET_H
