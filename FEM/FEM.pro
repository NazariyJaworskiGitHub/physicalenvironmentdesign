TEMPLATE = app
CONFIG += console
CONFIG += qt
CONFIG += c++11
CONFIG += no_keywords

QT += testlib

INCLUDEPATH += E:\ViennaCL-1.4.2\

SOURCES += main.cpp \
    boundarycondition.cpp \
    simulationobject.cpp \
    finiteelement.cpp \
    domain.cpp \
    beam.cpp \
    TESTS/test_finiteelement.cpp \
    grid.cpp \
    node.cpp \
    phase.cpp

HEADERS += \
    boundarycondition.h \
    simulationobject.h \
    finiteelement.h \
    domain.h \
    beam.h \
    TESTS/test_finiteelement.h \
    TESTS/tests_runner.h \
    grid.h \
    node.h \
    phase.h \
    utils.h
