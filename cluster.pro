# -------------------------------------------------
# Project created by QtCreator 2010-02-16T15:58:31
# -------------------------------------------------
TARGET = cluster
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    filelistmodel.cpp \
    datawrapper.cpp \
    modules/cb.c \
    modules/memctrl.c \
    modules/interfc.c \
    modules/file.c \
    process.cpp \
    processfactorymodel.cpp \
    processes/kmeans.cpp \
    processoptions.cpp \
    tsdata.cpp \
    cbdata.cpp \
    processoption.cpp \
    processoptions_types.cpp \
    datafilewidget.cpp \
    animation.cpp \
    aboutdialog.cpp \
    processdock.cpp \
    clusteringscene.cpp \
    datasetgraphicsitem.cpp
HEADERS += mainwindow.h \
    filelistmodel.h \
    datawrapper.h \
    modules/cb.h \
    modules/owntypes.h \
    modules/memctrl.h \
    modules/interfc.h \
    modules/file.h \
    process.h \
    processfactorymodel.h \
    processes/kmeans.h \
    processoptions.h \
    tsdata.h \
    cbdata.h \
    processoption.h \
    processoptions_types.h \
    datafilewidget.h \
    animation.h \
    aboutdialog.h \
    iconhelper.h \
    processdock.h \
    clusteringscene.h \
    datasetgraphicsitem.h
FORMS += mainwindow.ui \
    aboutdialog.ui
QMAKE_CXXFLAGS += -Werror \
    -g
OTHER_FILES += TODO.txt
RESOURCES += resources/cluster.qrc
