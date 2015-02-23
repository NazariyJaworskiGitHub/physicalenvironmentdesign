CONFIG += console
CONFIG += c++11
CONFIG += no_keywords

QT += testlib
QT += opengl

#Warning!!! this section is the including of FEM branch,
#remake project tree to avoid this section!!!
INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\MathUtils
INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\FEM

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
    TESTS/test_generator.cpp \
    simpleglrender3d.cpp \
    simpleglrender2d.cpp

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
    TESTS/test_generator.h \
    simpleglrender3d.h \
    simpleglrender2d.h \
    geometricobjects.h
