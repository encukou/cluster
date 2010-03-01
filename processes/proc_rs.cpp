#include "proc_rs.h"

#include <QtGui/QWidget>
#include <QtDebug>
#include "processoptions_types.h"

extern "C" {
#include "rls.h"
#include "modules/memctrl.h"
}

RandomSwap::RandomSwap(const ProcessOptionsPtr options, QObject* parent):
        Process(parent), options(options)
{}

void RandomSwap::process() {
    TSDataPtr ts = options->get<DataWrapperPtr>("input").dynamicCast<TSData>();
    CBDataPtr cb = options->get<DataWrapperPtr>("initial_cb").dynamicCast<CBData>();

    if(!ts) return;
    TRAININGSET* trainingset = ts->getDataCopy();

    int num_clusters = options->get<int>("cb_size");

    int useInitial;
    if (cb) {
        codebook = cb->getDataCopy();
        useInitial = 1; // 1 = use initial codebook, apparently
        num_clusters = cb->getDataSize();
    } else {
        codebook = (CODEBOOK*)allocate(sizeof(CODEBOOK));
        CreateNewCodebook(codebook, num_clusters, trainingset);
        useInitial = 0; // 0 = use initial partitioning, apparently
    }

    PARTITIONING* partitioning = (PARTITIONING*)allocate(sizeof(PARTITIONING));
    CreateNewPartitioning(partitioning, trainingset, num_clusters);

    PerformRLS(
            trainingset,                        // training set
            codebook,                           // codebook
            partitioning,                       // partitioning
            options->get<int>("no_iterations"), // iterations
            options->get<int>("kmeans_iter"),   // kmeans iterations
            0,                                  // deterministic
            0,                                  // quiet level
            useInitial                          // useInitial
        );

    // TODO: The function returns 0 on success and 1 on failure. Take that into account.

    FreeCodebook(codebook);
    deallocate(codebook);
    FreePartitioning(partitioning);
    deallocate(partitioning);
}

static ProcessResultTypeList types;

ProcessResultTypeList RandomSwap::resultTypes() const {
    if(types.empty()) {
        // TODO: Nicer API
        types.append(ProcessResultTypePtr(new ProcessResultType("output", tr("Codebook"), (QVariant::Type) QVariant::nameToType("CBDataPtr"))));
        types.append(ProcessResultTypePtr(new ProcessResultType("iteration_num", tr("Iteration number"), QVariant::Int)));
        types.append(ProcessResultTypePtr(new ProcessResultType("error", tr("Error"), QVariant::Int)));
    }
    return types;
}

QString RandomSwapFactory::name() const {
    return "Random Swap";
}

ProcessPtr RandomSwapFactory::newProcess(const ProcessOptionsPtr options, QObject* parent) const {
    return ProcessPtr(new RandomSwap(options, parent));
}

static ProcessOptionList opts;

ProcessOptionsPtr RandomSwapFactory::newOptions() const {
    if(opts.empty()) {
        opts.append((new TrainingSetOption("input", "Input training data"))->pointer());

        QStringList choices;
        choices << "Random" << "Existing Codebook";
        Q_ASSERT(choices.size() == INIT_COUNT); // Make sure the texts correspond to the enum values!
        opts.append((new EnumOption("init_type", "Initialization Method", choices, 0))->pointer());

        opts.append((new CodebookOption("initial_cb", "Initial codebook"))->pointer());

        IntOption* opt;
        opt = new IntOption("no_iterations", "No. of iterations", 5000, 1);
        opts.append(opt->pointer());

        opts.append((new IntOption("kmeans_iter", "No. of kmeans iterations", 2))->pointer());
        opts.append((new IntOption("cb_size", "No. of clusters", 256))->pointer());
    }
    return createNewOptions(opts);

}

ValidationResult RandomSwapFactory::validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged) {
    TSDataPtr input = options->get<DataWrapperPtr>("input").dynamicCast<TSData>(); // TODO: Better API
    InitType init_type = (InitType) options->get<int>("init_type");
    CBDataPtr initial_cb = options->get<DataWrapperPtr>("initial_cb").dynamicCast<CBData>();

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
