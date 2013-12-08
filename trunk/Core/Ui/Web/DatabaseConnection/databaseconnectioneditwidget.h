/// \author Nazariy Jaworski

#ifndef DATABASECONNECTIONEDITWIDGET_H
#define DATABASECONNECTIONEDITWIDGET_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <Wt/WDialog>
#include "Ui/Web/DatabaseConnection/databaseconnectionwidget.h"
#include "Ui/Web/DatabaseConnection/databaseconnectionitemsubwidget.h"
#include "Ui/Web/DatabaseConnection/databaseconnectioncreateconnectionitemwidget.h"

using namespace Wt;

namespace Ui
{
    namespace Web
    {
        namespace DatabaseConnection
        {
            /// Represents the user-interface for database connection \n
            /// <tt> +---------------------------------------------------------+ </tt>\n
            /// <tt> |Known connections:                                       | </tt>\n
            /// <tt> +---------------------------------------------------------+ </tt>\n
            /// <tt> |ConnectionName HostName DatabaseName UserName|Edit|Delete| </tt>\n
            /// <tt> |   ............                                          | </tt>\n
            /// <tt> +----------------------------+----------------------------+ </tt>\n
            /// <tt> |          Create            |            Exit            | </tt>\n
            /// <tt> +----------------------------+----------------------------+ </tt>\n
            /// Creates, edits or destroys database connection items by managing
            /// COOKIE_DATABASE_CONNECTIONS
            class DatabaseConnectionEditWidget : public WDialog
            {
                    /// Connection items, per line: ConnectionName HostName DatabaseName
                    /// UserName EditPushButton DeletePushButton
                private: QList<DatabaseConnectionItemSubWidget*> _myItems;
                    /// All items from _myItems will be stored here
                private: WContainerWidget *_myItemSubWidgetContainer = nullptr;
                    /// See _onItemCreate()
                private: WPushButton *_myCreateButton   = nullptr;
                    /// See _writeCookie()
                private: WPushButton *_myExitButton     = nullptr;
                    /// Calls DatabaseConnectionCreateConnectionItemWidget
                private: void _onItemCreate();
                    /// Clears _myItems, clears _myItemSubWidgetContainer,
                    /// reads COOKIE_DATABASE_CONNECTIONS, fills _myItems,
                    /// fills _myItemSubWidgetContainer
                private: void _readCookie();
                    /// Writes COOKIE_DATABASE_CONNECTIONS with _myItems data (cookie will
                    /// be saved during a month), emits accept() and closes the dialog
                private: void _writeCookie();

                    /// Common constructor
                public : DatabaseConnectionEditWidget(
                        WObject *wObjParent);
                    /// Common destructor
                public :~DatabaseConnectionEditWidget();
            };
        }
    }
}

#endif // DATABASECONNECTIONEDITWIDGET_H
