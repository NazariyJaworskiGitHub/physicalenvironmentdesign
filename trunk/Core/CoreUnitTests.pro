#/// \author Nazariy Jaworski

TEMPLATE    =   app

TARGET      =   CoreUnitTests

#QT dependences
CONFIG      +=  console
CONFIG      +=  no_keywords # no Qt-keywords for avoiding boost/Wt and Qt conflicts
CONFIG      +=  c++11
QT          +=  network
QT          +=  sql
QT          +=  testlib

#Wt Web ToolKit
INCLUDEPATH += -isystem E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\include\
INCLUDEPATH += -isystem E:\Boost\boost_1_54_0
LIBS += E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\lib\libwt.a
LIBS += E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\lib\libwttest.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_signals-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_system-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_thread-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_filesystem-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_regex-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_random-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_program_options-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_date_time-mgw48-mt-1_54.a
LIBS += -lws2_32
LIBS += -lwsock32

#Flags
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs #disable a lot of boost warnings
QMAKE_CXXFLAGS += -Wno-write-strings

#Versions
system("SubWCRev $$PWD $$PWD/version.h.tpl $$OUT_PWD/version.h") #Tortise Svn is used

SOURCES += \
    mainUnitTests.cpp \
    core.cpp \
    commandserver.cpp \
    guard.cpp \
    logger.cpp \
    Ui/Web/uiwebserver.cpp \
    Ui/Web/mainwidget.cpp \
    Ui/Web/authenticationwidget.cpp \
    Ui/Web/DatabaseConnection/dbcwidget.cpp \
    Ui/Web/DatabaseConnection/dbcitemsubwidget.cpp \
    Ui/Web/DatabaseConnection/dbceditwidget.cpp \
    Ui/Web/DatabaseConnection/dbccreateconnectionitemwidget.cpp \
    Session/project.cpp \
    Session/usersession.cpp \
    DataBase/databasemanager.cpp \
    DataBase/databaseupdater.cpp \
    workersession.cpp \
    UnitTests/test_uiwebserver.cpp \
    UnitTests/test_mainwidget.cpp \
    UnitTests/test_logger.cpp \
    UnitTests/test_guard.cpp \
    UnitTests/test_core.cpp \
    UnitTests/test_authenticationwidget.cpp \
    UnitTests/dummy_wserver.cpp \
    UnitTests/test_usersession.cpp

HEADERS += \
    core.h \
    commandserver.h \
    guard.h \
    logger.h \
    Ui/Web/uiwebserver.h \
    Ui/Web/mainwidget.h \
    Ui/Web/authenticationwidget.h \
    Ui/Web/DatabaseConnection/dbcwidget.h \
    Ui/Web/DatabaseConnection/dbcitemsubwidget.h \
    Ui/Web/DatabaseConnection/dbceditwidget.h \
    Ui/Web/DatabaseConnection/dbccreateconnectionitemwidget.h \
    Session/project.h \
    Session/usersession.h \
    DataBase/databasemanager.h \
    DataBase/databaseupdater.h \
    workersession.h \
    configurationparameters.h \
    UnitTests/unit_tests_runner.h \
    UnitTests/test_uiwebserver.h \
    UnitTests/test_mainwidget.h \
    UnitTests/test_logger.h \
    UnitTests/test_guard.h \
    UnitTests/test_core.h \
    UnitTests/dummy_wserver.h \
    UnitTests/test_authenticationwidget.h \
    UnitTests/test_usersession.h

OTHER_FILES += \
    Icons/mainicon.png \
    version.h.tpl

DEFINES +=  RUN_UNIT_TESTS
