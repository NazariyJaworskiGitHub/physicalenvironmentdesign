/// \file \author Nazariy Jaworski

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
#include "Ui/Web/DatabaseConnection/dbcwidget.h"

using namespace Wt;

namespace Ui
{
    namespace Web
    {
        namespace DatabaseConnection
        {
            /// Inline-widget for creation of a new connection data, can't be used
            /// without DBCEditWidget. Widget works only with given QStringList (see
            /// DBCCreateConnectionItemWidget() and _myConnectionData), and doesn't
            /// modify any other data
            class DBCCreateConnectionItemWidget : public WDialog
            {
                    /// Reference to external object
                private: QStringList &_myConnectionData;
                private: WLineEdit  *_myConnectionNameEdit   = nullptr;
                private: WLineEdit  *_myHostNameEdit         = nullptr;
                private: WLineEdit  *_myDatabaseNameEdit     = nullptr;
                private: WLineEdit  *_myUserNameEdit         = nullptr;
                private: WLineEdit  *_myPasswordEdit         = nullptr;
                    /// I f there is any user defined data, updates _myConnectionData,
                    /// emits accept() and closes the dialog
                private: void _onDoneButton();
                    /// New connection item data will be stored at connectionData,
                    /// which is the reference to external object
                    /// wObjParent currently not used
                public: DBCCreateConnectionItemWidget(
                        WObject *wObjParent,
                        QStringList &connectionData);
            };
        }
    }
}

#endif // DATABASECONNECTIONCREATECONNECTIONITEMWIDGET_H
