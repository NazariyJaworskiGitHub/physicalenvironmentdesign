TARGET  =   Core

#QT dependences
CONFIG  +=  console
CONFIG  +=  no_keywords
QT      +=  network

#Flags
QMAKE_CXXFLAGS += -std=c++0x

SOURCES +=  main.cpp \
    core.cpp \
    commandserver.cpp \
    guard.cpp

HEADERS += \
    core.h \
    commandserver.h \
    guard.h


