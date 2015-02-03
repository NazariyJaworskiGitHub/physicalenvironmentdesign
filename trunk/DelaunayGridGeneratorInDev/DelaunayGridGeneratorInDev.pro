CONFIG += console
CONFIG += c++11
CONFIG += no_keywords

QT += testlib
QT += opengl

#Warning!!! this section is the including of FEM branch,
#remake project tree to avoid this section!!!
INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\FEM

SOURCES += main.cpp \
    simpleglrender.cpp \
    piecewiselinearcomplex.cpp \
    TESTS/test_piecewiselinearcomplex.cpp

HEADERS += \
    simpleglrender.h \
    piecewiselinearcomplex.h \
    TESTS/tests_runner.h \
    TESTS/test_piecewiselinearcomplex.h
