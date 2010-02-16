# -------------------------------------------------
# Project created by QtCreator 2010-02-16T15:58:31
# -------------------------------------------------
TARGET = cluster
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    filelistmodel.cpp
    datawrapper.cpp \
    modules/cb.c \
    modules/memctrl.c \
    modules/interfc.c \
    modules/file.c
HEADERS += mainwindow.h \
    filelistmodel.h
    datawrapper.h \
    modules/cb.h \
    modules/owntypes.h \
    modules/memctrl.h \
    modules/interfc.h \
    modules/file.h
FORMS += mainwindow.ui
