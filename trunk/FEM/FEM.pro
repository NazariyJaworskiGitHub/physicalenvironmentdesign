CONFIG += console
CONFIG += qt
CONFIG += c++11
CONFIG += no_keywords

QT += testlib

#For Qt containers usage
#DEFINES += _USE_QT_CONTAINERS

#For debugging
CONFIG(debug, release|debug):DEFINES += _DEBUG_MODE

#INCLUDEPATH += E:\ViennaCL-1.4.2\

#Warning!!! this section is the including of FEM branch,
#remake project tree to avoid this section!!!
INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\MathUtils

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
    domain.cpp \
    beam.cpp \
    grid.cpp \
    phase.cpp \
    cuboid.cpp \
    TESTS/test_beam.cpp \
    simplexelement.cpp \
    TESTS/test_simplexelement.cpp

HEADERS += \
    boundarycondition.h \
    simulationobject.h \
    domain.h \
    beam.h \
    TESTS/tests_runner.h \
    grid.h \
    phase.h \
    cuboid.h \
    TESTS/test_beam.h \
    simplexelement.h \
    TESTS/test_simplexelement.h
