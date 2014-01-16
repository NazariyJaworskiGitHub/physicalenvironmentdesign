TEMPLATE = app
CONFIG += console
CONFIG += qt
CONFIG += c++11
CONFIG += no_keywords

QT += testlib

SOURCES += main.cpp \
    boundarycondition.cpp \
    simulationobject.cpp \
    finiteelement.cpp \
    domain.cpp \
    beam.cpp \
    TESTS/test_finiteelement.cpp \
    grid.cpp \
    node.cpp \
    environment.cpp

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
    environment.h
