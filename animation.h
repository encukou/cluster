#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include "processoptions.h"

class Animation: public QObject, public ProcessOptionsValidator {
public:
    ProcessOptionsPtr newOptions() const;
    ValidationResult validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged);
private:
    mutable QSharedPointer<Animation> m_ptr;
};

#endif // ANIMATION_H
