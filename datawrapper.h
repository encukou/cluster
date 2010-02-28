#ifndef DATAWRAPPER_H
#define DATAWRAPPER_H

#include <QString>
#include <QtCore/QSharedPointer>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>

extern "C" {
#include "modules/cb.h"
}

class DataWrapper;
typedef QSharedPointer<DataWrapper> DataWrapperPtr;
Q_DECLARE_METATYPE(DataWrapperPtr);

/** A C++ object that represents a dataset (training set, codebook, etc)
  *
  * Datasets that are wrapped by this class should NOT be modified (i.e. don't give them to the
  * clustering algorithms written in C). Each subclass should provide a getDataCopy() method
  * that etracts a modifiable copy of the data.
  *
  * Datasets are usually loaded from a file. The fromFile function does that for training sets and
  * codebooks. For partitionings, call the PAData constructor explicitly (see its documentation for reasons).
  *
  * Datasets have a filename (the file from which they were loaded or to which they were last saved),
  * a name (this appears in the file list, etc.), and a suggestedFilename (for generated files,
  * this is the default file name for the Save dialog).
  *
  * Generated datafiles – that is, those that were not loaded but are the result of some algorithm – have
  * an empty filePath. When saved, they start behaving exactly like normally opened files.
  *
  * DataWrapperPtr is a typedef for QSharedPointer<DataWrapper>. Use this when handling DataWrapper,
  * so you don't need to worry about deallocation. When the last DataWrapperPtr is destroyed,
  * the corresponding data is deleted as well.
  */
class DataWrapper {
public:
    CBFILETYPE getType() { return dataType; }
    virtual void* getData() = 0;
    virtual int getDataSize() = 0;
    virtual void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0) = 0;
    virtual int getVectorSize() = 0;
    virtual bool save(QString filename) const = 0;

    QString name();
    QString filePath();

    static DataWrapperPtr fromFile(QString fileName);
    static CBFILETYPE getFileType(QString fileName);

    QString suggestedFilename;

protected:
    CBFILETYPE dataType;
    void setFileName(QString fileName, QString stdExtension=QString());
    QString m_fileName;
    QString m_filePath;
};

#endif // DATAWRAPPER_H
