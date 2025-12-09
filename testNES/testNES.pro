include(../NES.pro)

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += qt
QT += core

SOURCES += \
        main.cpp

DESTDIR = $$BIN_DIR

DISTFILES += \
    benchmark_log.txt
