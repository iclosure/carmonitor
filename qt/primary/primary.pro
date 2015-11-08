################################################################
# Project created by QtCreator 2013-06-04T13:26:00
#
# Copyright (C) 2014 Smartsoft
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Smartsoft, version 1.0
################################################################

include( config.pri )
include( src/src.pri )

################################################################
# project config
################################################################

QT += core gui

TEMPLATE = app
TARGET = $$qtLibraryTarget(CarMonitor)
DESTDIR = $${PWD}/../output/bin

######################################################################
# qmake internal options
######################################################################

#CONFIG += thread static
#CONFIG += static
CONFIG -= app_bundle

######################################################################
# release/debug mode
######################################################################

win32 {
    CONFIG(release, debug|release): {
        DEFINES += QT_NO_DEBUG_OUTPUT
    }
} else {
    CONFIG += release
    DEFINES += QT_NO_DEBUG_OUTPUT
}

linux-g++ {
    CONFIG += separate_debug_info
}

################################################################
# creating a precompiled header file (only supported on some platforms (Windows - all MSVC project types,
# Mac OS X - Xcode, Makefile, Unix - gcc 3.3 and up)
################################################################

CONFIG += precompile_header
PRECOMPILED_HEADER = precomp.h
INCLUDEPATH += .

################################################################
# Resouces
################################################################

RESOURCES += \
    resource/image/res_image.qrc \
    resource/lang/res_lang.qrc \
    resource/qss/res_qss.qrc

################################################################
# Translations
################################################################

TRANSLATIONS += \
    resource/lang/en_US.ts \
    resource/lang/zh_CN.ts

################################################################
# Others
################################################################

OTHER_FILES += \
    $${DESTDIR}/../data/*.xml \
    resource/qml/res_qml.qrc

################################################################
# import libraries
################################################################

## import Qwt library

win32:CONFIG(release, debug|release): LIBS += -L$$(SMARTKITS_HOME)/Qt/Qwt-6.1.1/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(SMARTKITS_HOME)/Qt/Qwt-6.1.1/lib/ -lqwtd
else:unix: LIBS += -L$$(SMARTKITS_HOME)/Qt/Qwt-6.1.1/lib/ -lqwt

INCLUDEPATH += $$(SMARTKITS_HOME)/Qt/Qwt-6.1.1/include
DEPENDPATH += $$(SMARTKITS_HOME)/Qt/Qwt-6.1.1/include
