CONFIG += console
CONFIG += c++11
CONFIG += no_keywords

QT += testlib

#For Qt containers usage
#DEFINES += _USE_QT_CONTAINERS

#For debugging
CONFIG(debug, release|debug):DEFINES += _DEBUG_MODE

#For dr.memory debug
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
    TESTS/test_precision.cpp \
    TESTS/test_node.cpp \
    TESTS/test_mathutils.cpp \
    TESTS/test_extendedreal.cpp \
    TESTS/test_finiteelement.cpp

HEADERS += \
    MathUtils \
    realdeclaration.h \
    simplematrix.h \
    node.h \
    extendedreal.h \
    FUNCTIONS/factorial.h \
    FUNCTIONS/rand.h \
    FUNCTIONS/round.h \
    FUNCTIONS/trunc.h \
    FUNCTIONS/calculatecircumspherecenter.h \
    FUNCTIONS/calculateiscoplanarstatus.h \
    FUNCTIONS/calculategeneralizedcrossproduct.h \
    FUNCTIONS/calculatesimplexvoulumebycayleymengerdeterminant.h \
    FUNCTIONS/calculatebarycentriccoordinates.h \
    FUNCTIONS/calculatesegmentsubsimplexbarycenticintersection.h \
    FUNCTIONS/calculatesubsimplexsubsimplexintersection.h \
    printmachineinfo.h \
    TESTS/tests_runner.h \
    TESTS/test_precision.h \
    TESTS/test_node.h \
    TESTS/test_mathutils.h \
    TESTS/test_extendedreal.h \
    finiteelement.h \
    TESTS/test_finiteelement.h \
    containerdeclaration.h

