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
    processes/kmeans.c \
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
    datasetgraphicsitem.cpp \
    modules/cnvxhull.c \
    padata.cpp \
    modules/random.c \
    processes/proc_kmeans.cpp \
    processes/rls.c \
    reporting.cpp \
    datawrappermime.cpp \
    processresultsmodel.cpp \
    clusteringitemdelegate.cpp \
    modules/voronoi.c \
    modules/stack.c \
    modules/sort.c
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
    datasetgraphicsitem.h \
    modules/cnvxhull.h \
    padata.h \
    modules/random.h \
    processes/proc_kmeans.h \
    processes/rls.h \
    reporting.h \
    datawrappermime.h \
    processresultsmodel.h \
    clusteringitemdelegate.h \
    modules/voronoi.h \
    modules/stack.h \
    modules/sort.h
FORMS += mainwindow.ui \
    aboutdialog.ui
QMAKE_CXXFLAGS += -Werror \
    -g
QMAKE_CFLAGS += -g \
    -Imodules/
OTHER_FILES += TODO.txt
RESOURCES += resources/cluster.qrc
