CONFIG += console
CONFIG += c++11
CONFIG += no_keywords

QT += testlib

#INTEL OpenCL
INCLUDEPATH += E:\OpenCL\Intel\include
LIBS += -LE:\OpenCL\Intel\lib\x86\ -lOpenCL
#AMD OpenCL
LIBS += E:\OpenCL\AMD\lib\x86\libOpenCL.a

#Wt Web ToolKit
INCLUDEPATH += E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\include\
INCLUDEPATH += E:\Boost\boost_1_54_0
LIBS += E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\lib\libwt.a
LIBS += E:\Wt\Wt_3.3.0_boost_1.54.0_mingw_4.8.0_x32\lib\libwthttp.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_signals-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_system-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_thread-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_filesystem-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_regex-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_random-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_program_options-mgw48-mt-1_54.a
LIBS += E:\Boost\boost_1_54_0\lib\libboost_date_time-mgw48-mt-1_54.a
LIBS += -lws2_32
LIBS += -lwsock32

#Flags
QMAKE_CXXFLAGS += -Wno-unused-local-typedefs #disable a lot of boost warnings
QMAKE_CXXFLAGS += -Wno-write-strings

# For dr.memory debug
Debug:QMAKE_CXXFLAGS += -ggdb

#Warning!!! this section is the including of FEM branch,
#remake project tree to avoid this section!!!
INCLUDEPATH += E:\Developing\BPSPO\PhysicalEnvironmentDesign\FEM

SOURCES += main.cpp \
    generator.cpp \
    UTILS/gridrender.cpp \
    piecewiselinearcomplex.cpp \
    datamanager.cpp \
    TESTS/test_piecewiselinearcomplex.cpp \
    GPU/openclmanager.cpp \
    nodewrapper.cpp \
    TESTS/test_gridelement.cpp \
    gridelement.cpp \
    TESTS/test_nodewrapper.cpp

HEADERS += \
    TESTS/tests_runner.h \
    generator.h \
    UTILS/utilwebserver.h \
    UTILS/utilwebapp.h \
    UTILS/gridrender.h \
    piecewiselinearcomplex.h \
    datamanager.h \
    TESTS/test_piecewiselinearcomplex.h \
    GPU/openclmanager.h \
    GPU/clsources.h \
    nodewrapper.h \
    TESTS/test_gridelement.h \
    gridelement.h \
    TESTS/test_nodewrapper.h \
    listwrapperinterface.h
