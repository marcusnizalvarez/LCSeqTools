# ------------ FLAGS
QT -= gui
LIBS += -pthread
CONFIG += c++11 console
CONFIG -= app_bundle
DEFINES += QT_NO_DEPRECATED_WARNINGS
DEFINES += GIT_REVISION='\\"$$system(git rev-parse --short HEAD)\\"'
# ------------ SOURCE
SOURCES += \
    src/lcseqtools_cli.cpp \
    src/main.cpp
HEADERS += \
    src/lcseqtools_cli.h
RESOURCES += \
    src/cli_resource.qrc

# ------------ BUILD
TARGET = LCSeqTools_cli
DESTDIR = ../build/
OBJECTS_DIR = ../build/cli
MOC_DIR = ../build/cli
RCC_DIR = ../build/cli
UI_DIR = ../build/cli
