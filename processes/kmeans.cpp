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
        Q_ASSERT(choices.size() == INIT_COUNT); // Make sure the texts correspond to the enum values!
        opts.append((new EnumOption("init_type", "Initialization Method", choices, 0))->pointer());

        //opts.append((new CodebookOption("initial_cb", "Initial codebook"))->pointer());

        IntOption* opt;
        opt = new IntOption("no_iterations", "No. of iterations");
        opt->specialValueText = "(Until convergence)";
        opts.append(opt->pointer());

        opts.append((new IntOption("cb_size", "No. of clusters", 256))->pointer());
    }
    return ProcessOptions::newOptions(this->pointer(), opts);

}

bool KMeansFactory::validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged) {
    TSDataPtr input = options->get<TSDataPtr>("input");
    CBDataPtr initial_cb = options->get<CBDataPtr>("initial_cb");
    InitType init_type = (InitType) options->get<int>("init_type");

    // Check init_type is inside bounds
    if(init_type < 0 || init_type >= INIT_COUNT) {
        // Bad initialization type
        if(lastChanged->name == "init") return options->setDefault(lastChanged);
        return false;
    }

    // Check for valid initialization type (INIT_CB with a codebook, or something else without one)
    if(initial_cb) {
        if(init_type != INIT_CB) {
            if(lastChanged->name == "init_type") {
                return options->set("initial_cb", QVariant());
            }else if(lastChanged->name == "initial_cb") {
                return options->set("init_type", INIT_CB);
            }else{
                return false;
            }
        }
    }else{
        if(init_type == INIT_CB) {
            if(lastChanged->name == "init_type") {
                return false;
            }else if(lastChanged->name == "initial_cb") {
                return options->setDefault("init_type");
            }else{
                return false;
            }
        }
    }

    // Check that we have input
    if(!input) return false;

    // All OK
    return true;
}
