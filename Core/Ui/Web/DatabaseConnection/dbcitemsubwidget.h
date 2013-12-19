/// \file \author Nazariy Jaworski

#ifndef DATABASECONNECTIONITEMSUBWIDGET_H
#define DATABASECONNECTIONITEMSUBWIDGET_H

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
            /// Inline-widget for connection data representation, can't be used
            /// without DBCEditWidget
            class DBCItemSubWidget : public WContainerWidget
            {
                public : QStringList myConnectionData;
                private: WLabel      *_myConnectionDataLabel = nullptr;
                private: WPushButton *_myEditButton          = nullptr;
                private: WPushButton *_myDeleteButton        = nullptr;
                    /// Calls DBCCreateConnectionItemWidget
                private: void _onEditButton();
                    /// Emits sDelete
                private: void _onDeleteButton();
                    /// Catch this with parent object to destroy current object
                public : Signal<DBCItemSubWidget*> sDelete;
                    /// Updates the _myConnectionDataLabel with myConnectionData
                private: void _update();
                    /// Common constructor, you have to set parent pointer
                public: DBCItemSubWidget(
                        WContainerWidget *parent,
                        QStringList connectionData);

            };
        }
    }
}

#endif // DATABASECONNECTIONITEMSUBWIDGET_H
