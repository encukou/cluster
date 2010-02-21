#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include "processoptions.h"

class Animation: public QObject {
public:
    Animation(QObject* parent=0);
    static ProcessOptionsPtr newOptions();
    ValidationResult validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged);
};

typedef QSharedPointer<Animation> AnimationPtr;

#endif // ANIMATION_H
