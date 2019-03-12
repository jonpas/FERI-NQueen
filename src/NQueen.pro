QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NQueen
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    localsearch.cpp

HEADERS += \
    mainwindow.h \
    localsearch.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    rsc/crown.png

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
