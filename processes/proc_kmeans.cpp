#include "proc_kmeans.h"

#include <QtGui/QWidget>
#include <QtDebug>
#include "processoptions_types.h"

extern "C" {
#include "kmeans.h"
#include "modules/memctrl.h"
}

KMeans::KMeans(const ProcessOptionsPtr options, QObject* parent):
        Process(parent), options(options)
{}

void KMeans::process() {
    TSDataPtr ts = options->get<DataWrapperPtr>("input").dynamicCast<TSData>();
    CBDataPtr cb = options->get<DataWrapperPtr>("initial_cb").dynamicCast<CBData>();

    if(!ts) return;
    TRAININGSET* trainingset = ts->getDataCopy();

    int num_clusters = options->get<int>("cb_size");

    int useInitial;
    if(cb) {
        codebook = cb->getDataCopy();
        useInitial = 1; // 1 = use initial codebook, apparently
    }else{
        codebook = (CODEBOOK*)allocate(sizeof(CODEBOOK));
        CreateNewCodebook(codebook, num_clusters, trainingset);
        useInitial = 0; // 0 = use initial partitioning, apparently
    }

    PARTITIONING* partitioning = (PARTITIONING*)allocate(sizeof(PARTITIONING));
    CreateNewPartitioning(partitioning, trainingset, num_clusters);

    PerformKMeans(
            trainingset,                    // training set
            codebook,                       // codebook
            partitioning,                   // partitioning
            num_clusters,                   // clus
            1,                              // repeats
            options->get<int>("init_type"), // init method
            0,                              // quiet level
            useInitial                      // useInitial
        );

    // TODO: The function returns 0 on success and 1 on failure. Take that into account.

    FreeCodebook(codebook);
    deallocate(codebook);
    FreePartitioning(partitioning);
    deallocate(partitioning);
}

static ProcessResultTypeList types;

ProcessResultTypeList KMeans::resultTypes() const {
    if(types.empty()) {
        // TODO: Nicer API
        types.append(ProcessResultTypePtr(new ProcessResultType("output", tr("Codebook"), (QVariant::Type) QVariant::nameToType("CBDataPtr"))));
        types.append(ProcessResultTypePtr(new ProcessResultType("error", tr("Error"), QVariant::Int)));
    }
    return types;
}

QString KMeansFactory::name() const {
    return "K-means";
}

ProcessPtr KMeansFactory::newProcess(const ProcessOptionsPtr options, QObject* parent) const {
    return ProcessPtr(new KMeans(options, parent));
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
    TSDataPtr input = options->get<DataWrapperPtr>("input").dynamicCast<TSData>(); // TODO: Better API
    InitType init_type = (InitType) options->get<int>("init_type");
    CBDataPtr initial_cb = options->get<DataWrapperPtr>("initial_cb").dynamicCast<CBData>();
    int cb_size = options->get<int>("cb_size");

    ValidationResult result;

    // Check init_type is inside bounds
    if(init_type < 0 || init_type >= INIT_COUNT) {
        // Bad initialization type
        if(lastChanged->name == "init" && options->setDefault(lastChanged)) return true;
        options->validationError(result, tr("Bad initialization method"), "init_type");
    }

    if(initial_cb) {
        // Check that the codebook size is consistent with the chosen CB size.
        if(cb_size != initial_cb->getDataSize()) {
            ValidationResult subresult;
            if(lastChanged) {
                subresult = options->set("cb_size", initial_cb->getDataSize());
            }
            if(subresult.badElements.contains(options->getOption("cb_size"))) {
                options->validationError(result, tr("The number of clusters does not correspond to the given codebook (size %1)!").arg(initial_cb->getDataSize()), "cb_size");
            }
        }
        // Check for valid initialization type (only INIT_CB can be used with a codebook)
        if(init_type != INIT_CB) {
            if(lastChanged->name == "init_type" && options->set("initial_cb", QVariant())) return true;
            if(lastChanged->name == "initial_cb" && options->set("init_type", INIT_CB)) return true;
            options->validationError(result, tr("The given initial codebook will be unused!"), "init_type initial_cb");
        }
        if(input && initial_cb->getVectorSize() != input->getVectorSize()) {
            options->validationError(result, tr("The dimensionalities of the training set and initial codebook are not the same!"), "initial_cb input");
        }
    }else{
        // Check for valid initialization type (INIT_CB can only be used with a codebook)
        if(init_type == INIT_CB) {
            if(lastChanged->name == "initial_cb" && options->setDefault("init_type")) return true;
            options->validationError(result, tr("No initial codebook given!"), "init_type initial_cb");;
        }
    }

    // Check that we have input
    if(!input) options->validationError(result, tr("No training data given!"), "input");

    // All OK
    return result;
}
