TARGET = resize-app

TEMPLATE = app

CONFIG += console

SOURCES += main.cpp 

include(../../common.pri)

QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-unused-parameter -std=c++11 -O3

