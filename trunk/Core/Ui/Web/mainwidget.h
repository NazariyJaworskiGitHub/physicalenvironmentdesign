/// \file \author Nazariy Jaworski

#ifndef UIWEBMAINWIDGET_H
#define UIWEBMAINWIDGET_H

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
#include <Wt/WEnvironment>

#include "Ui/Web/authenticationwidget.h"
#include "Session/usersession.h"

using namespace Wt;

namespace Ui
{
    namespace Web
    {
        /// Main widget for web users \n
        /// (actually this is not a widget, but the WApplication, which corresponds
        /// to a user web-session, and contains the WApplication::root() main widget) \n
        /// All ui web widgets are connected to this widget tree, all of them have Q_SIGNAL
        /// writeString(), so they inherit QObject, but all of their QObject parents
        /// should be nullptrs, becouse the are in different threads. Thats why, you
        /// need to check objects destructions
        /// \todo needs code review, check all public members, maybe they have to be private?
        /// \todo it class stores and processes UserSession, bu this is Ui, maybe it
        /// should be different?
        /// \todo make some specific class for WMessageBox info widget that will catch
        /// writeString
        /// \todo signals from widget tree

        // +----------------------------------------------------+
        // | LogIn/LogOut widget                                |
        // +----------------------------------------------------+
        // | Working area                                       |
        // |    TAB    |    TAB    |    TAB    |                |
        // +-----------+-----------+-----------+----------------+
        // |                                                    |
        // |                                                    |
        // |                                                    |
        // |                                                    |
        // +----------------------------------------------------+
        // |Contact information, license,etc.                   |
        // +----------------------------------------------------+

        class MainWidget : public QObject, public WApplication
        {
            Q_OBJECT

                /// Represents working UserSession for current user (i.e. web session)
                /// if you call delete, dont forget to set it with nullptr
                /// \todo i don't shure that this item have to be located here
                /// \todo make it as singleton for web-session, like WApplication::instance()
            private: Session::UserSession* _myUserSession = nullptr;
            public : const Session::UserSession* getUserSession() const {return _myUserSession;}
                /// Authentication widget
            private: AuthenticationWidget *_myUiWebAuthenticationWidget = nullptr;
            private: WTabWidget *_myWorkingAreaTabWidget = nullptr;
                /// Common constructor
                /// parent currently not used
            public : MainWidget(const WEnvironment &env, QObject *parent = nullptr);
                /// Common destructor
            public : ~MainWidget();
                /// Catch this signal with some Ui or Logger
            public : Q_SIGNAL void writeString(const QString message) const;
        };
    }
}

#endif // UIWEBMAINWIDGET_H

