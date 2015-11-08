################################################################
# carmonitor application
#
# Copyright (C) 2014 Smartsoft
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Smartsoft License, Version 1.0
################################################################

contains(APP_CONFIG, uitools) {
    QT += uitools
}

contains(APP_CONFIG, QwtDll) {
    win32|symbian: DEFINES += QWT_DLL
}
!contains(APP_CONFIG, QwtDll) {
    win32|symbian: DEFINES += QWT_DLL_UNDEF
}

contains(APP_CONFIG, sql) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += sql
    } else {
        message("sql not supported on Qt version " + QT_VERSION)
    }
}

contains(APP_CONFIG, widgets) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += widgets
    } else {
        message("widgets not supported on Qt version " + QT_VERSION)
    }
}

contains(APP_CONFIG, winextras) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += winextras
    } else {
        message("winextras not supported on Qt version " + QT_VERSION)
    }
}

contains(APP_CONFIG, webkit) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += webkit
    } else {
        message("webkit not supported on Qt version " + QT_VERSION)
    }
}

contains(APP_CONFIG, webkitwidgets) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += webkitwidgets
    } else {
        message("webkitwidgets not supported on Qt version " + QT_VERSION)
    }
}

contains(APP_CONFIG, serialport) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += serialport
    } else {
        message("serialport not supported on Qt version " + QT_VERSION)
    }
}

contains(APP_CONFIG, quick) {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += qml quick
    } else {
        QT += declarative
    }
}

################################################################
# import sources in subdirectory
################################################################

################################################################
# import sources in current directory
################################################################

HEADERS += \
    src/main_widget.h \
    src/database/databasemgr.h \
    src/protocol/protocol.h \
    src/protocol/serialport_proxy.h \
    src/curve_widget.h \
    src/curve.h \
    $$PWD/slider_widget.h \
    $$PWD/settings_widget.h \
    $$PWD/history_widget.h

SOURCES += \
    src/main.cpp \
    src/main_widget.cpp \
    src/database/databasemgr.cpp \
    src/protocol/serialport_proxy.cpp \
    src/curve_widget.cpp \
    src/curve.cpp \
    $$PWD/slider_widget.cpp \
    $$PWD/settings_widget.cpp \
    $$PWD/history_widget.cpp

################################################################
# import target sources in current directory
################################################################

################################################################
# import simulate sources
################################################################

CONFIG(debug, debug|release): {

    DEFINES += SERIALPORT_SIMULATE

    HEADERS += \
        src/simulate/serialport_simulate.h

    SOURCES += \

}
