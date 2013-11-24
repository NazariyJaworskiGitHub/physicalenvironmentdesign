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

    public: WServer *myWServer = nullptr;
        ///< Wt web server
    private: static WApplication *createApplication(const WEnvironment& env);
        ///< creates a new user session at myWServer
    public: UiWebServer(QObject *parent);
        ///< Common constructor, don't forget to call startServer()
    public: ~UiWebServer();
        ///< Common destructor
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
    public : Q_SIGNAL void serverStarted() const;
        ///< is emmiting when server has been started
    public : Q_SLOT void startServer();
        ///< slot for server starting
};

#endif // UIWEBSERVER_H
