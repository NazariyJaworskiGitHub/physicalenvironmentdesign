TEMPLATE = app
CONFIG += console
CONFIG += qt
CONFIG += c++11
CONFIG += no_keywords

QT += testlib

#INCLUDEPATH += E:\ViennaCL-1.4.2\

INCLUDEPATH += e:\Boost\boost_1_54_0\

# For dr.memory debug
Debug:QMAKE_CXXFLAGS += -ggdb

SOURCES += main.cpp \
    boundarycondition.cpp \
    simulationobject.cpp \
    finiteelement.cpp \
    domain.cpp \
    beam.cpp \
    TESTS/test_finiteelement.cpp \
    grid.cpp \
    node.cpp \
    phase.cpp \
    cuboid.cpp \
    TESTS/test_node.cpp \
    TESTS/test_beam.cpp \
    TESTS/test_precision.cpp

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
    cuboid.h \
    TESTS/test_node.h \
    mathutils.h \
    TESTS/test_beam.h \
    TESTS/test_precision.h
