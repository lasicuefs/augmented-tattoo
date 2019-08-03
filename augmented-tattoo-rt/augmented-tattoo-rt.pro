TARGET = augmentedTattooRT

TEMPLATE = app

#CONFIG += console

SOURCES += main.cpp \
    ui/mainwindow.cpp \
    betterblobdetector.cpp \
    tattoorenderer.cpp \
    utils.cpp \
    glew/glew.c

HEADERS  += \
    ui/mainwindow.h \
    blob.h \
    matchshapeengine.h \
    contourextractor.h \
    centralmarkerdetector.h \
    thresholding.h \
    lgriddetector.h \
    lshapeengine.h \
    circularmarkerdetector.h \
    occlusionhandler.h \
    markermatcher.h \
    betterblobdetector.h \
    imagewarper.h \
    imagemanager.h \
    tattoorenderer.h \
    glew/GL/glew.h \
    glew/GL/glxew.h \
    glew/GL/wglew.h \
    shader.h \
    tattoodrawer.h \
    timestats.h \
    utils.h

QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-unused-parameter -std=c++11 -O3

include(../common.pri)

FORMS += \
    ui/mainwindow.ui \
    ui/about.ui

INCLUDEPATH += glew
