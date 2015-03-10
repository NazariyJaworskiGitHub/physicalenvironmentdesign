CONFIG += console
CONFIG += c++11
CONFIG += no_keywords

QT += testlib
QT += opengl

#For GLEW
win32{
    win32-g++:contains(QMAKE_HOST.arch, x86_64):{
        LIBS += "C:\Program Files (x86)\AMD APP SDK\2.9-1\lib\x86_64\libglew64.dll.a"
    } else {
        LIBS += "C:\Program Files (x86)\AMD APP SDK\2.9-1\lib\x86\libglew32.dll.a"
    }
}

#For Qt containers usage
#DEFINES += _USE_QT_CONTAINERS

#Warning!!! this section is the including of FEM branch,
#remake project tree to avoid this section!!!
#Warning 2!!!!!!
#can't compile Eigen 3.2.0 and (GLEW + OpenCL) see https://bugreports.qt.io/browse/QTBUG-38312
INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\MathUtils
#INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\FEM

#INTEL OpenCL
#INCLUDEPATH += E:\OpenCL\Intel\include
#LIBS += E:\OpenCL\Intel\lib\x86\OpenCL.lib

#AMD OpenCL
INCLUDEPATH += E:\OpenCL\AMD\include
win32{
    win32-g++:contains(QMAKE_HOST.arch, x86_64):{
        LIBS += "C:\Program Files (x86)\AMD APP SDK\2.9-1\lib\x86_64\libOpenCL.a"
    } else {
        LIBS += "C:\Program Files (x86)\AMD APP SDK\2.9-1\lib\x86\libOpenCL.a"
    }
}

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
    CLMANAGER/clmanager.cpp \
    CLMANAGER/TESTS/test_clmanager.cpp \
    volumeglrender.cpp \
    representativevolumeelement.cpp

HEADERS += \
    CLMANAGER/clmanager.h \
    CLMANAGER/TESTS/tests_runner.h \
    CLMANAGER/TESTS/test_clmanager.h \
    volumeglrender.h \
    representativevolumeelement.h
