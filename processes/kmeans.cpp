#include "kmeans.h"

#include <QtGui/QWidget>

KMeansFactory::KMeansFactory() {
}

QString KMeansFactory::name() const {
    return "K-means";
}

QSharedPointer<Process> KMeansFactory::newProcess(const ProcessOptions&) const {
}
