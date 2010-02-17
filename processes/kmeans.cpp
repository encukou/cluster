#include "kmeans.h"

#include <QtGui/QWidget>
#include <QtGui/QWidget>
#include "processoptions.h"

static QSharedPointer<ProcessOptions> options;

KMeansFactory::KMeansFactory() {
}

QString KMeansFactory::name() const {
    return "K-means";
}

ProcessPtr KMeansFactory::newProcess(const ProcessOptionsPtr options) const {
    KMeans* p = new KMeans(
            options->get<QSharedPointer<TSData> >("input"),
            options->get<QSharedPointer<CBData> >("initial_cb"),
            options->get<int>("init_type"),
            options->get<int>("cb_size")
        );

    return ProcessPtr(p);
}

QSharedPointer<ProcessOptions> KMeansFactory::getOptions() const {
    if(!options) {
        QList<ProcessOption*> opts;
        IntOption* opt;
        opts.append(new EnumOption("init_type", "Initialization Method", QStringList() << "Random" << "K-Means++" << "Marko's Division to Strips" << "Existing Codebook", 0));
        opt = new IntOption("no_iterations", "No. of iterations");
        opt->specialValueText = "(Until convergence)";
        opts.append(opt);
        opts.append(new IntOption("cb_size", "No. of clusters", 256));
        options = ProcessOptions::newOptions(opts);
    }
    return options;
}
