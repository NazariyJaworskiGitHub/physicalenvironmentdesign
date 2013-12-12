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
#include <QStringList>
#include <Wt/WApplication>
#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WDialog>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include "Session/usersession.h"

/// 5 items in the connection data:
/// ConnectionName, HostName, DatabaseName, UserName, Password
/// many connection data in the cookie
#define COOKIE_DATABASE_CONNECTIONS "DATABASECONNECTIONS"

using namespace Wt;

namespace Ui
{
    namespace Web
    {
        namespace DatabaseConnection
        {
            /// Represents the user-interface for database connection \n
            /// <tt> +------------+------------+ </tt>\n
            /// <tt> |Connections |  ComboBox  | </tt>\n
            /// <tt> +------------+------------+ </tt>\n
            /// <tt> |HostName    |............| </tt>\n
            /// <tt> |DatabaseName|............| </tt>\n
            /// <tt> |UserName    |............| </tt>\n
            /// <tt> +--------+---+---+--------+ </tt>\n
            /// <tt> |Connect | Edit  | Cancel | </tt>\n
            /// <tt> +--------+-------+--------+ </tt>\n
            /// Data about database connections is stored at client side
            /// in COOKIE_DATABASE_CONNECTIONS \n
            /// When some error is found, emits reject()
            /// \todo some bug with destruction
            class DBCWidget : public QObject, public WDialog
            {
                Q_OBJECT
                    /// If this pointer is nonzero, data of COOKIE_DATABASE_CONNECTIONS
                    /// be dublicatet here (i need this, because i can't refresh cookies
                    /// in session).
                private: std::string *_myInternalStorage = nullptr;
                    /// Stores connection data,
                    /// data can be selected by _myConnectionsComboBox
                private: QList<QStringList> _myConnectionsData;
                    /// Use to store index of selected connection from myConnectionsData,
                    /// or \c -1 if last is empty
                private: int _myIndexOfSelectedConnection;
                    /// Should point to Session::UserSession* of top-level widget,
                    /// See DatabaseConnectionWidget()
                private: Session::UserSession * const _myUserSession;

                private: WLabel     *_myConnectionsLabel     = nullptr;
                private: WLabel     *_myHostNameLabel        = nullptr;
                private: WLabel     *_myDatabaseNameLabel    = nullptr;
                private: WLabel     *_myUserNameLabel        = nullptr;

                    /// See _fillConnectionsList(), see _myConnectionsData,
                    /// see _onConnectionsItemChange()
                private: WComboBox  *_myConnectionsComboBox  = nullptr;

                private: WLineEdit  *_myHostNameEdit         = nullptr;
                private: WLineEdit  *_myDatabaseNameEdit     = nullptr;
                private: WLineEdit  *_myUserNameEdit         = nullptr;

                    /// See _onConnectButton()
                private: WPushButton *_myConnectButton       = nullptr;
                    /// See _onEditButton()
                private: WPushButton *_myEditButton          = nullptr;
                    /// See _onCancelButton()
                private: WPushButton *_myCancelButton        = nullptr;

                    /// Common constructor,
                    /// qObjParent currently not used
                    /// wObjParent currently not used
                public : DBCWidget(
                        Session::UserSession * const ptrToUserSession,
                        QObject *qObjParent,
                        WObject *wObjParent);
                    /// Fills myConnectionsComboBox by saved COOKIE_DATABASE_CONNECTIONS:\n
                    /// 1) clear the _myConnectionsComboBox, clear the myConnectionsData;\n
                    /// 2) try to read the COOKIE_DATABASE_CONNECTIONS; \n
                    /// 3) if exist and not empty, fill _myConnectionsData, fill the
                    /// _myConnectionsComboBox, make the _myConnectButton->enable(),
                    /// set the _myIndexOfSelectedConnection = 0;\n
                    /// 4) elese, make the _myConnectButton->disable(),
                    /// set the _myIndexOfSelectedConnection = -1, add "No known connections"
                    /// item to the _myConnectionsComboBox;
                    /// \todo need refactoring, internal storage excludes cookie
                private: void _fillConnectionsList(
                        bool readFromInternalStorage,
                        std::string *internalStorage);
                    /// When user try to connect to database, finishes the dialog by emiting
                    /// accept()
                private: void _onConnectButton();
                    /// Finishes the dialog by emiting reject()
                private: void _onCancelButton();
                    /// Calls DBCEditWidget
                private: void _onEditButton();
                    /// When user change the item at _myConnectionsComboBox
                private: void _onConnectionsItemChange();

                    /// Common destructor
                public : ~DBCWidget();
                    /// Catch this signal with some Ui or Logger
                public : Q_SIGNAL void writeString(const QString message) const;
            };
        }
    }
}

#endif // UIWEBDATABASECONNECTIONWIDGET_H
