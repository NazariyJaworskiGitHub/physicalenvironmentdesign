TARGET  =   Ui

#QT dependences
CONFIG  +=  console
CONFIG  +=  no_keywords
QT      +=  network

#Flags
QMAKE_CXXFLAGS += -std=c++0x

SOURCES +=  main.cpp \
    ui.cpp

HEADERS += \
    ui.h
