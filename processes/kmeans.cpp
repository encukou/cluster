#include "kmeans.h"

#include <QtGui/QWidget>
#include <QtGui/QWidget>
#include "processoptions_types.h"

KMeans::KMeans(TSDataPtr trainingset, CBDataPtr initial_codebook, int initialization, int no_clusters) {
    // TODO!
    (void) trainingset;
    (void) initial_codebook;
    (void) initialization;
    (void) no_clusters;
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

QList<ProcessOptionPtr> opts;

ProcessOptionsPtr KMeansFactory::getOptions() const {
    if(!opts.size()) {
        opts.append((new TrainingSetOption("input", "Input training data"))->pointer());

        QStringList choices;
        choices << "Random" << "K-Means++" << "Marko's Division to Strips" << "Existing Codebook";
        opts.append((new EnumOption("init_type", "Initialization Method", choices, 0))->pointer());

        IntOption* opt;
        opt = new IntOption("no_iterations", "No. of iterations");
        opt->specialValueText = "(Until convergence)";
        opts.append(opt->pointer());

        opts.append((new IntOption("cb_size", "No. of clusters", 256))->pointer());
    }
    return ProcessOptions::newOptions(opts);
}
