TEMPLATE = app
QT += svg
TARGET = plottsp
INCLUDEPATH += .
QMAKE_CXXFLAGS += -g -Wall -pedantic -std=gnu++0x

# For Qt 5.x we need the widgets module.
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

# Input
SOURCES += plottsp.cpp view.cpp
HEADERS += view.h vertex.h

mac {
  CONFIG -= app_bundle
}
