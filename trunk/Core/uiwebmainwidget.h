/// \author Nazariy Jaworski

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

#include "uiwebauthenticationwidget.h"
#include "usersession.h"

using namespace Wt;

/// main widget for web users \n
/// \todo needs code review, check all public members, maybe they have to be private?
/// \todo it class stores and processes UserSession, bu this is Ui, maybe it should be different?
/// \todo make some specific class for WMessageBox info widget that will catch writeString
/// \todo signals from widget tree \n
/// all ui web widgets are connected to this widget tree, all of them have Q_SIGNAL writeString(),
/// so they inherit QObject, but all of their QObject parents should be nullptrs, becouse the are in
/// different threads. Thats why, you need to check objects destructions
class UiWebMainWidget : public QObject, public WApplication
{
    Q_OBJECT

        /// Represents working UserSession for current user (i.e. web session)
        /// if you call delete, dont forget to set it with nullptr
        /// \todo i don't shure that this item have to be located here
        /// \todo make it as singleton for web-session, like WApplication::instance()
    private: UserSession* _myUserSession = nullptr;
        /// Authentication widget
    public : UiWebAuthenticationWidget *myUiWebAuthenticationWidget = nullptr;
        /// Common constructor
        /// parent currently not used
    public : UiWebMainWidget(const WEnvironment &env, QObject *parent);
        /// Common destructor
    public : ~UiWebMainWidget();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;

    /*public : Q_SLOT void createUserSession(Guard::UserData *ptrToUserData);
        ///< makes a new UserSession at _myUserSession, emits sessionCreated()
    public : Q_SIGNAL void sessionCreated() const;
        ///< emit when a new session is created
    public : Q_SLOT void destroyUserSession();
        ///< destroys the UserSession at _myUserSession, automaticaly logOut, emits sessionDestroed()
    public : Q_SIGNAL void sessionDestroed() const;
        ///< emit when the session is destroed

    public : Signal<> sessionCreated_s;
    public : Signal<> sessionDestroed_s;*/
};

#endif // UIWEBMAINWIDGET_H

