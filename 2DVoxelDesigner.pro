#-------------------------------------------------
#
# Project created by QtCreator 2019-10-05T00:10:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "2D Voxel Designer v2.0"
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    graphicsview.cpp \
    node.cpp \
    imageitem.cpp \
    cornergrabber.cpp \
    newscenedlg.cpp \
    edge.cpp \
    widget.cpp \
    shapeline.cpp \
    movingarrow.cpp \
    arrow.cpp \
    baseshape.cpp \
    movingarc.cpp \
    shapecornertocornerrectangle.cpp \
    shapecenterpointcircle.cpp \
    shapecentertocornerrectangle.cpp \
    shape3pointcircle.cpp \
    shapecenteroval.cpp \
    shapecenterarc.cpp \
    shape3pointoval.cpp \
    shape3pointarc.cpp \
    shapepolygon.cpp \
    shapehandlecurve.cpp \
    shapepointcurve.cpp \
    shapepointhandlecurve.cpp \
    markerdialog.cpp \
    markeritem.cpp \
    coordinateform.cpp \
    bezierinterpolator.cpp \
    voxelpolygon.cpp \
    customlayerbutton.cpp \
    layerdata.cpp

HEADERS += \
    mainwindow.h \
    graphicsview.h \
    node.h \
    imageitem.h \
    cornergrabber.h \
    newscenedlg.h \
    edge.h \
    widget.h \
    shapeline.h \
    movingarrow.h \
    arrow.h \
    baseshape.h \
    movingarc.h \
    shapecornertocornerrectangle.h \
    shapecenterpointcircle.h \
    shapecentertocornerrectangle.h \
    shape3pointcircle.h \
    shapecenteroval.h \
    shapecenterarc.h \
    shape3pointoval.h \
    shape3pointarc.h \
    shapepolygon.h \
    shapehandlecurve.h \
    shapepointhandlecurve.h \
    shapepointcurve.h \
    markerdialog.h \
    markeritem.h \
    coordinateform.h \
    bezierinterpolator.h \
    voxelpolygon.h \
    customlayerbutton.h \
    layerdata.h

FORMS += \
    mainwindow.ui \
    newscenedlg.ui \
    markerdialog.ui \
    widget.ui \
    coordinateform.ui \
    customlayerbutton.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
win32:RC_FILE += resource.rc
