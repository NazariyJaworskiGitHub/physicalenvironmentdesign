#/// \author Nazariy Jaworski

TARGET  =   Core
ICON    =   icons\mainicon.png

#QT dependences
CONFIG      +=  console
CONFIG      +=  no_keywords # no Qt-keywords for avoiding boost/Wt and Qt conflicts
QT          +=  network
QT          +=  sql

#Wt Web ToolKit
INCLUDEPATH += -isystem E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\include\
INCLUDEPATH += -isystem E:\Boost\boost_1_54_0
LIBS += E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\lib\libwt.a
LIBS += E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\lib\libwthttp.a
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
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs #disable a lot of boost warnings
QMAKE_CXXFLAGS += -Wno-write-strings

#Versions
system("SubWCRev $$PWD $$PWD/version.h.tpl $$OUT_PWD/version.h") #Tortise Svn is used

SOURCES +=  main.cpp \
    core.cpp \
    commandserver.cpp \
    guard.cpp \
    session.cpp \
    logger.cpp \
    uiwebserver.cpp \
    uiwebmainwidget.cpp \
    uiwebauthenticationwidget.cpp \
    project.cpp \
    usersession.cpp \
    uiwebdatabaseconnectionwidget.cpp \
    databasemanager.cpp

HEADERS += \
    core.h \
    commandserver.h \
    guard.h \
    session.h \
    logger.h \
    uiwebserver.h \
    uiwebmainwidget.h \
    version.h.tpl \
    uiwebauthenticationwidget.h \
    project.h \
    usersession.h \
    uiwebdatabaseconnectionwidget.h \
    databasemanager.h

OTHER_FILES += \
    icons/mainicon.png


