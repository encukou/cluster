#ifndef NICEDRAGTREEVIEW_H_
#define NICEDRAGTREEVIEW_H_

#include <QTreeView>

/// TreeView with a nice drag icon; the default is sometimes too ugly.
class NiceDragTreeView: public QTreeView {
public:
    NiceDragTreeView(QWidget* parent=0): QTreeView(parent) {}
protected:
    void startDrag(Qt::DropActions supportedActions);
};

#endif
