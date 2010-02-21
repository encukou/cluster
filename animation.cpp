#include "animation.h"
#include "processoptions_types.h"

Animation::Animation(QObject* parent): QObject(parent) {
}

static ProcessOptionList opts;
static ProcessOptionsValidator dummyValidator;
static QSharedPointer<ProcessOptionsValidator> dummyValidatorPtr(&dummyValidator);

ProcessOptionsPtr Animation::newOptions() {
    if(!opts.size()) {
        opts.append((new BoolOption("on", "Enable animation", true))->pointer());

        opts.append((new BoolOption("on", "Stop at first frame", false))->pointer());

        IntOption* opt;
        opt = new IntOption("timer", tr("Animation timer"), 0, 0, 100000);
        opt->specialValueText = "(None)";
        opt->suffix = " ms";
        opt->step = 100;
        opts.append(opt->pointer());
    }
    return ProcessOptions::newOptions(dummyValidatorPtr, opts);
}

ValidationResult Animation::validateOptions(ProcessOptionsPtr, ProcessOptionPtr) {
    // All OK
    return true;
}
