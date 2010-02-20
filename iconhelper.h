#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QtGui/QIcon>

QIcon loadIcon(QString category, QString name) {
    // Qt 4.6 includes loading standard system icons on Linux
    // (http://doc.trolltech.com/4.6/qicon.html#fromTheme)
    //
    // However, Qt 4.6 still doesn't integrate this functionality
    // into the UI Designer (http://bugreports.qt.nokia.com/browse/QTBUG-7777)
    // so a function like this will be needed.
    //
    // We want to support all versions of from Qt 4.5.
    //
    // TODO: write an #ifdef for Qt 4.6 using QIcon::fromTheme, adapt
    // the qrc file, and test it

    return QIcon(":/icons/silk/16x16/" + category + "/" + name);
}

#endif // ICONHELPER_H
