#-------------------------------------------------
#
# Project created by QtCreator 2015-03-05T13:23:13
#
#-------------------------------------------------

QT       += core gui

TARGET = raytracing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    view.cpp \
    phong.cpp \
    render.cpp \
    meshobject.cpp \
    kdtree.cpp \
    pathtracing.cpp \
    meshsimplification.cpp

HEADERS  += mainwindow.h \
    constant.h \
    color.h \
    geometry.h \
    view.h \
    material.h \
    light.h \
    object.h \
    global.h \
    thread.h \
    phong.h \
    render.h \
    meshobject.h \
    kdtree.h \
    aabb.h \
    texture.h \
    pathtracing.h \
    meshsimplification.h

FORMS    += mainwindow.ui
