#include "animation.h"
#include "processoptions_types.h"

static ProcessOptionList opts;

ProcessOptionsPtr Animation::newOptions() const {
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
    if(!m_ptr) m_ptr = QSharedPointer<Animation>((Animation*)this);
    return ProcessOptions::newOptions(m_ptr.dynamicCast<ProcessOptionsValidator>(), opts);
}

ValidationResult Animation::validateOptions(ProcessOptionsPtr, ProcessOptionPtr) {
    // All OK
    return true;
}
