#include "proc_kmeans.h"

#include <QtGui/QWidget>
#include <QtDebug>
#include "processoptions_types.h"

KMeans::KMeans(TSDataPtr trainingset, CBDataPtr initial_codebook, int initialization, int no_clusters, QObject* parent):
        Process(parent)
{
    // TODO!
    (void) trainingset;
    (void) initial_codebook;
    (void) initialization;
    (void) no_clusters;
}

void KMeans::process() {
    // TODO

    // dummy process:
    reportIterationResult(ProcessResults());
    reportIterationResult(ProcessResults());
    reportIterationResult(ProcessResults());
    sleep(1);
    reportIterationResult(ProcessResults());
}

static ProcessResultTypeList types;

ProcessResultTypeList KMeans::resultTypes() const {
    if(types.empty()) {
        types.append(ProcessResultTypePtr(new ProcessResultType("codebook", (QVariant::Type) QVariant::nameToType("CBDataPtr"))));
        types.append(ProcessResultTypePtr(new ProcessResultType("mse", QVariant::Int)));
    }
    return types;
}

QString KMeansFactory::name() const {
    return "K-means";
}

ProcessPtr KMeansFactory::newProcess(const ProcessOptionsPtr options, QObject* parent) const {
    KMeans* p = new KMeans(
            options->get<QSharedPointer<TSData> >("input"),
            options->get<QSharedPointer<CBData> >("initial_cb"),
            options->get<int>("init_type"),
            options->get<int>("cb_size"),
            parent
        );

    return ProcessPtr(p);
}

static ProcessOptionList opts;

ProcessOptionsPtr KMeansFactory::newOptions() const {
    if(opts.empty()) {
        opts.append((new TrainingSetOption("input", "Input training data"))->pointer());

        QStringList choices;
        choices << "Random" << "K-Means++" << "Marko's Division to Strips" << "Existing Codebook";
        Q_ASSERT(choices.size() == INIT_COUNT); // Make sure the texts correspond to the enum values!
        opts.append((new EnumOption("init_type", "Initialization Method", choices, 0))->pointer());

        opts.append((new CodebookOption("initial_cb", "Initial codebook"))->pointer());

        IntOption* opt;
        opt = new IntOption("no_iterations", "No. of iterations");
        opt->specialValueText = "(Until convergence)";
        opts.append(opt->pointer());

        opts.append((new IntOption("cb_size", "No. of clusters", 256))->pointer());
    }
    return createNewOptions(opts);

}

ValidationResult KMeansFactory::validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged) {
    TSDataPtr input = options->get<TSDataPtr>("input");
    InitType init_type = (InitType) options->get<int>("init_type");
    CBDataPtr initial_cb = options->get<CBDataPtr>("initial_cb");

    ValidationResult result;

    // Check init_type is inside bounds
    if(init_type < 0 || init_type >= INIT_COUNT) {
        // Bad initialization type
        if(lastChanged->name == "init" && options->setDefault(lastChanged)) return true;
        options->validationError(result, tr("Bad initialization method"), "init_type");
    }

    // Check for valid initialization type (INIT_CB with a codebook, or something else without one)
    if(initial_cb) {
        if(init_type != INIT_CB) {
            if(lastChanged->name == "init_type" && options->set("initial_cb", QVariant())) return true;
            if(lastChanged->name == "initial_cb" && options->set("init_type", INIT_CB)) return true;
            options->validationError(result, tr("The given initial codebook will be unused!"), "init_type initial_cb");
        }
    }else{
        if(init_type == INIT_CB) {
            if(lastChanged->name == "initial_cb" && options->setDefault("init_type")) return true;
            options->validationError(result, tr("No initial codebook given!"), "init_type initial_cb");;
        }
    }

    // TODO: Check that the number of clusters (cb_size) is consistent with initial_cb

    // Check that we have input
    if(!input) options->validationError(result, tr("No training data given!"), "input");

    // All OK
    return result;
}
