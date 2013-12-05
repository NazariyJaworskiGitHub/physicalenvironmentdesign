/// \author Nazariy Jaworski

#ifndef UIWEBDATABASECONNECTIONWIDGET_H
#define UIWEBDATABASECONNECTIONWIDGET_H

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
#include <Wt/WDialog>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include "usersession.h"

using namespace Wt;

/// Represents the user-interface for database connection
// +------------+------------+
// |HostName    |............|
// |DatabaseName|............|
// |UserName    |............|
// |Password    |............|
// +------------+------------+
// |Connect     |Cancel      |
// +------------+------------+
class UiWebDatabaseConnectionWidget : public QObject, public WDialog
{
    Q_OBJECT

        /// Should point to UserSession* of top-level widget, see UiWebAuthenticationWidget()
    private: UserSession * const _myUserSession;

    public : WLabel     *myHostNameLabel        = nullptr;
    public : WLabel     *myDatabaseNameLabel    = nullptr;
    public : WLabel     *myUserNameLabel        = nullptr;
    public : WLabel     *myPasswordLabel        = nullptr;

    public : WLineEdit  *myHostNameEdit         = nullptr;
    public : WLineEdit  *myDatabaseNameEdit     = nullptr;
    public : WLineEdit  *myUserNameEdit         = nullptr;
    public : WLineEdit  *myPasswordEdit         = nullptr;

    public : WPushButton *myConnectButton       = nullptr;
    public : WPushButton *myCancelButton        = nullptr;

        /// Common constructor,
        /// qObjParent currently not used
    public : UiWebDatabaseConnectionWidget(
            UserSession * const ptrToUserSession,
            QObject *qObjParent,
            WObject *wObjParent);
        /// When user try to connect to database
    public : void onConnectButton();
        /// When user pushes Cancel
    public : void onCancelButton();
        /// Common destructor
    public : ~UiWebDatabaseConnectionWidget();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
};

#endif // UIWEBDATABASECONNECTIONWIDGET_H
