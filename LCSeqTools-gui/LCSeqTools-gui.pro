# ------------ FLAGS
QT       += core gui
QT       += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS
#QMAKE_CXXFLAGS += -finstrument-functions

# ------------ SOURCE
SOURCES += \
    src/fastqcviewer.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/parameterswindow.cpp \
    src/referenceeditor.cpp \
    src/samplemanager.cpp \
    src/statswindow.cpp
HEADERS += \
    src/fastqcviewer.h \
    src/mainwindow.h \
    src/parameterswindow.h \
    src/referenceeditor.h \
    src/samplemanager.h \
    src/statswindow.h
FORMS += \
    src/fastqcviewer.ui \
    src/mainwindow.ui \
    src/parameterswindow.ui \
    src/referenceeditor.ui \
    src/samplemanager.ui \
    src/statswindow.ui
RESOURCES += \
    src/gui_resource.qrc

# ------------ BUILD
INCLUDEPATH += ../LCSeqTools-cli/src
target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

TARGET = LCSeqTools
DESTDIR = ../build/
LIBS += ../build/cli/lcseqtools_cli.o
LIBS += ../build/cli/moc_lcseqtools_cli.o
LIBS += ../build/cli/qrc_cli_resource.o
OBJECTS_DIR = ../build/gui
MOC_DIR = ../build/gui
RCC_DIR = ../build/gui
UI_DIR = ../build/gui
