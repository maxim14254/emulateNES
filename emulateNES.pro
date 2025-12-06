#QT       += core gui openglwidgets opengl
QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_ICONS = nintendoNES.ico

LOG = 1

equals(LOG, 1) {
    DEFINES += LOG_ON
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += $$files(src/*.cpp) \
    src/log.cpp

HEADERS += $$files(src/*.h) \
    src/log.h

FORMS += $$files(forms/*)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
