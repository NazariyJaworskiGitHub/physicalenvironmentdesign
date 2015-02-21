TEMPLATE = app
CONFIG += console
CONFIG += qt
CONFIG += c++11
CONFIG += no_keywords

QT += testlib

#INCLUDEPATH += E:\ViennaCL-1.4.2\

INCLUDEPATH += e:\Boost\boost_1_54_0\

# For dr.memory debug
QMAKE_CXXFLAGS_DEBUG += -ggdb

#Optimisation flags
#see https://gcc.gnu.org/onlinedocs/gcc-4.9.2/gcc/Optimize-Options.html#Optimize-Options
#    except -ftree-loop-vectorize for -O3 level (at least, not supportet in MinGW 4.8.0)
#Note, -fipa-cp-clone if disabled, because it causes the fail at MathUtils::
#    calculateCircumSphereCenterByCayleyMengerDeterminant::Eigen::DynamicMatrix::lu().solve
#    at least under MinGW 4.8.0 and Eigen 3.2.0
QMAKE_CXXFLAGS_RELEASE -= -O1
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE *= -O3
QMAKE_CXXFLAGS_RELEASE += -O2 \ #Equivalent to -O3 (MinGW 4.8.0)
    -finline-functions \        #includes all, except -fipa-cp-clone
    -funswitch-loops \
    -frename-registers \
    -fpredictive-commoning \
    -fgcse-after-reload \
    -ftree-slp-vectorize \
    -fvect-cost-model \
    -ftree-partial-pre

#QMAKE_CXXFLAGS_DEBUG -= -O1
##QMAKE_CXXFLAGS_DEBUG -= -O2
##QMAKE_CXXFLAGS_DEBUG *= -O3
#QMAKE_CXXFLAGS_DEBUG += -O2 \   #Equivalent to -O3 (MinGW 4.8.0)
#    -finline-functions \        #includes all, except -fipa-cp-clone
#    -funswitch-loops \
#    -frename-registers \
#    -fpredictive-commoning \
#    -fgcse-after-reload \
#    -ftree-slp-vectorize \
#    -fvect-cost-model \
#    -ftree-partial-pre

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
    TESTS/test_precision.cpp \
    simplexelement.cpp \
    TESTS/test_mathutils.cpp \
    TESTS/test_extendedreal.cpp

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
    TESTS/test_precision.h \
    simplexelement.h \
    TESTS/test_mathutils.h \
    extendedreal.h \
    TESTS/test_extendedreal.h
