/// \author Nazariy Jaworski

#ifndef UIWEBSERVER_H
#define UIWEBSERVER_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <QThread>

#include <Wt/WServer>
#include <Wt/WApplication>

#include "uiwebmainwidget.h"

using namespace Wt;

/// Implements web user interface http server, at its own thread
class UiWebServer : public QObject
{
    Q_OBJECT

        /// Wt web server
    public: WServer *myWServer = nullptr;
        /// Creates a new user session at myWServer
        /// \todo this function should be common for all types of Ui
        /// \todo put here the creation of UserSession object instead of UiWebMainWidget
    private: static WApplication *createApplication(const WEnvironment& env);
        /// Common constructor, don't forget to call startServer(),
        /// configuration file for WServer is \file wt_config.xml
    public : UiWebServer(QObject *parent);
        /// Common destructor
    public: ~UiWebServer();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
        /// Is emmiting when server has been started
    public : Q_SIGNAL void serverStarted() const;
        /// Slot for server starting
    public : Q_SLOT void startServer();     
};

#endif // UIWEBSERVER_H
