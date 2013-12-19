/// \file \author Nazariy Jaworski

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
#include "Ui/Web/DatabaseConnection/dbcwidget.h"
#include "Ui/Web/DatabaseConnection/dbcitemsubwidget.h"
#include "Ui/Web/DatabaseConnection/dbccreateconnectionitemwidget.h"

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
            /// \todo it too difficult, needs refactor
            class DBCEditWidget : public WDialog
            {
                    /// If true (and any _myInternalStorage is given), initial data be
                    /// readed from _myInternalStorage but not from
                    /// COOKIE_DATABASE_CONNECTIONS, default false
                private: bool _myReadFromInternalStorageFlag;
                    /// If this pointer is nonzero, data of COOKIE_DATABASE_CONNECTIONS
                    /// be dublicatet here (i need this, because i can't refresh cookies
                    /// in session). See _myReadFromInternalStorageFlag and DBCEditWidget()
                private: std::string *_myInternalStorage = nullptr;
                    /// Connection items, per line: ConnectionName HostName DatabaseName
                    /// UserName EditPushButton DeletePushButton
                private: QList<DBCItemSubWidget*> _myItems;
                    /// All items from _myItems will be stored here
                private: WContainerWidget *_myItemSubWidgetContainer = nullptr;
                    /// See _onItemCreate()
                private: WPushButton *_myCreateButton   = nullptr;
                    /// See _writeCookie()
                private: WPushButton *_myExitButton     = nullptr;
                    /// Adds QStringList item to _myItems and _myItemSubWidgetContainer
                private: void _addItem(QStringList item);
                    /// Calls DBCCreateConnectionItemWidget
                private: void _onItemCreate();
                    /// Is called when DBCCreateConnectionItemWidget::sDelete was emited.
                    /// Destroys caller and updates dialog
                private: void _onItemDelete(DBCItemSubWidget *caller);
                    /// Clears _myItems, clears _myItemSubWidgetContainer,
                    /// reads COOKIE_DATABASE_CONNECTIONS, fills _myItems,
                    /// fills _myItemSubWidgetContainer
                private: void _readCookie();
                    /// Writes COOKIE_DATABASE_CONNECTIONS with _myItems data (cookie will
                    /// be saved during a month), emits accept() and closes the dialog
                private: void _writeCookie();
                    /// Common constructor
                    /// Set internalStorage to nullptr if you work with
                    /// COOKIE_DATABASE_CONNECTIONS, or nonzero if you works wiht
                    /// _myInternalStorage. See _myReadFromInternalStorageFlag
                public : DBCEditWidget(
                        bool readFromInternalStorageFlag = false,
                        WObject *wObjParent = nullptr,
                        std::string *internalStorage = nullptr);
                    /// Common destructor
                public :~DBCEditWidget();
            };
        }
    }
}

#endif // DATABASECONNECTIONEDITWIDGET_H
