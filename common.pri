QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32|win32-g++:DEFINES += _CRT_SECURE_NO_WARNINGS

win32|win32-g++ {
    INCLUDEPATH += C:/opencv-build-2.4.13/install/include
    INCLUDEPATH += C:/Qt/Qt5.8.0/Tools/mingw530_32/i686-w64-mingw32/include/GL
    LIBS += -LC:/opencv-build-2.4.13/install/x86/mingw/bin \
        -lopencv_core2413 \
        -lopencv_contrib2413 \
        -lopencv_calib3d2413 \
        -lopencv_highgui2413 \
        -lopencv_imgproc2413 \
        -lopencv_gpu2413 \
        -lopencv_features2d2413 \
        -lopencv_flann2413 \
        -lopencv_legacy2413 \
        -lopencv_ml2413 \
        -lopencv_video2413 \
        -lopencv_videostab2413 \
        -lopencv_photo2413 \
        -lopencv_nonfree2413 \
        -lopengl32
}

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv
}




