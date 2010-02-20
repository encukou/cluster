#include "process.h"
#include "processoptions.h"


ProcessFactoryPtr ProcessFactory::pointer() {
    if(_ptr) {
        return ProcessFactoryPtr(_ptr);
    }else{
        ProcessFactoryPtr p = ProcessFactoryPtr(this);
        _ptr = p;
        return p;
    }
}

const ProcessFactoryPtr ProcessFactory::pointer() const {
    return const_cast<ProcessFactory*>(this)->pointer();
}

ProcessOptionsPtr ProcessFactory::createNewOptions(ProcessOptionList opts) const {
    return ProcessOptions::newOptions(this->pointer().dynamicCast<ProcessOptionsValidator>(), opts);
}
