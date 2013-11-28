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
    private: UserSession ** const _myUserSession;
        ///< should point to UserSession* of top-level widget, see UiWebAuthenticationWidget()

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

    public : UiWebDatabaseConnectionWidget(
            UserSession ** const ptrToUserSession,
            QObject *qObjParent,
            WObject *wObjParent);
        ///< Common constructor
    public : void onConnectButton();
        ///< when user try to connect to database
    public : ~UiWebDatabaseConnectionWidget();
        ///< Common destructor
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // UIWEBDATABASECONNECTIONWIDGET_H
