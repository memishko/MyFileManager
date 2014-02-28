#-------------------------------------------------
#
# Project created by QtCreator 2013-12-03T13:18:02
#
#-------------------------------------------------

QT       += core gui

TARGET = myFileManager
TEMPLATE = app

LIBS +=ole32.lib\
        shell32.lib



SOURCES += main.cpp\
        mainwindow.cpp \
    myfilemgmodel.cpp

HEADERS  += mainwindow.h \
    myfilemgmodel.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    pics.qrc
