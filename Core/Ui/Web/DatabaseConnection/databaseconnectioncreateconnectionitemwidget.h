/// \author Nazariy Jaworski

#ifndef DATABASECONNECTIONCREATECONNECTIONITEMWIDGET_H
#define DATABASECONNECTIONCREATECONNECTIONITEMWIDGET_H

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

using namespace Wt;

namespace Ui
{
    namespace Web
    {
        namespace DatabaseConnection
        {
            /// Inline-widget for creation of a new connection data, can't be used
            /// without DatabaseConnectionEditWidget
            class DatabaseConnectionCreateConnectionItemWidget : public WDialog
            {
                private: QStringList &_myConnectionData;
                private: WLineEdit  *_myConnectionNameEdit   = nullptr;
                private: WLineEdit  *_myHostNameEdit         = nullptr;
                private: WLineEdit  *_myDatabaseNameEdit     = nullptr;
                private: WLineEdit  *_myUserNameEdit         = nullptr;
                private: WLineEdit  *_myPasswordEdit         = nullptr;
                private: void _onCreateButton();
                    /// New connection item data will be stored at connectionData
                public: DatabaseConnectionCreateConnectionItemWidget(
                        WObject *wObjParent,
                        QStringList &connectionData);
            };
        }
    }
}

#endif // DATABASECONNECTIONCREATECONNECTIONITEMWIDGET_H
