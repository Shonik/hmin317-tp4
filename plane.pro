QT       += core gui widgets

TARGET = TP3
TEMPLATE = app

SOURCES += main.cpp \
    modelply.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    modelply.h

RESOURCES += \
    shaders.qrc \
    textures.qrc

# install
target.path = .
INSTALLS += target
