CONFIG += console
CONFIG += c++11
CONFIG += no_keywords

QT += testlib
QT += opengl

# For dr.memory debug
Debug:QMAKE_CXXFLAGS += -ggdb

#Warning!!! this section is the including of FEM branch,
#remake project tree to avoid this section!!!
INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\FEM

SOURCES += main.cpp \
    simpleglrender.cpp \
    piecewiselinearcomplex.cpp \
    TESTS/test_piecewiselinearcomplex.cpp \
    nodewrapper.cpp \
    TESTS/test_nodewrapper.cpp \
    generator.cpp \
    gridelement.cpp \
    TESTS/test_gridelement.cpp \
    datamanager.cpp \
    TESTS/test_datamanager.cpp \
    TESTS/test_generator.cpp

HEADERS += \
    simpleglrender.h \
    piecewiselinearcomplex.h \
    TESTS/tests_runner.h \
    TESTS/test_piecewiselinearcomplex.h \
    nodewrapper.h \
    TESTS/test_nodewrapper.h \
    listwrapperinterface.h \
    generator.h \
    gridelement.h \
    TESTS/test_gridelement.h \
    datamanager.h \
    TESTS/test_datamanager.h \
    TESTS/test_generator.h
