#include "animation.h"
#include "processoptions_types.h"
#include <QtDebug>

Animation::Animation(ProcessOptionsPtr animOptions, QObject* parent): QObject(parent) {
    if(animOptions->get<bool>("start_stopped")) {
        state = ANIM_STOPPED;
    }else{
        state = ANIM_WAITING;
    }
    m_timerValue = animOptions->get<int>("timeout_value");
    m_frame = -1;
    m_lastLoadedFrame = -1;
    m_lastFrame = -1;
    connect(this, SIGNAL(playbackStarted()), SIGNAL(stateChanged()));
    connect(this, SIGNAL(playbackEnded()), SIGNAL(stateChanged()));
    connect(this, SIGNAL(lastFrameChanged(int)), SIGNAL(stateChanged()));
    connect(this, SIGNAL(frameChanged(int)), SIGNAL(stateChanged()));
    connect(this, SIGNAL(lastLoadedFrameChanged(int)), SIGNAL(stateChanged()));
}

static ProcessOptionList opts;

ProcessOptionsPtr Animation::newOptions() {
    static QSharedPointer<ProcessOptionsValidator> dummyValidatorPtr(new ProcessOptionsValidator);
    if(!opts.size()) {
        // TODO: Implement disabling animation
        //opts.append((new BoolOption("on", "Enable animation", true))->pointer());

        opts.append((new BoolOption("start_stopped", "Stop at first frame", false))->pointer());

        IntOption* opt;
        opt = new IntOption("timeout_value", tr("Animation timer"), 200, 0, 100000);
        opt->specialValueText = "(None)";
        opt->suffix = " ms";
        opt->step = 100;
        opts.append(opt->pointer());
    }
    return ProcessOptions::newOptions(dummyValidatorPtr, opts);
}

bool Animation::toNextFrame() { return setFrame(frame()+1); }
bool Animation::toPreviousFrame() { return setFrame(frame()-1); }
bool Animation::toFirstFrame() { return setFrame(0); }
bool Animation::toLastFrame() { return setFrame(lastLoadedFrame()); }

bool Animation::play() {
    if(playing() || atLastFrame()) return false;
    startTimer();
    emit playbackStarted();
    return true;
}

bool Animation::pause() {
    if(!playing()) return false;
    stopTimer();
    emit playbackEnded();
    return true;
}

bool Animation::playPause() {
    if(playing()) {
        return pause();
    }else{
        if(atLastFrame()) toFirstFrame();
        return play();
    }
}

bool Animation::setFrame(int newFrame) {
    int origParam = newFrame;
    if(newFrame < 0) newFrame = 0;
    if(newFrame > lastLoadedFrame()) newFrame = lastLoadedFrame();
    if(newFrame < 0) return false; // No frames loaded...
    if(newFrame == m_frame) return true; // No change
    m_frame = newFrame;
    emit frameChanged(newFrame);
    return origParam == newFrame;
}

int Animation::frame() { return m_frame; }
bool Animation::playing() { return state != ANIM_STOPPED; }
bool Animation::waiting() { return state == ANIM_WAITING; }
int Animation::lastLoadedFrame() { return m_lastLoadedFrame; }
int Animation::lastFrame() { return m_lastFrame; }
bool Animation::isLoadingDone() { return m_lastLoadedFrame == m_lastFrame && !moreFramesExpected(); }

bool Animation::atLastFrame() {
    return frame() == m_lastFrame && isLoadingDone();
}

void Animation::setLastLoadedFrame(int newFrame) {
    if(newFrame == m_lastLoadedFrame) return;
    Q_ASSERT(newFrame > m_lastLoadedFrame);
    if(newFrame > m_lastFrame) setLastFrame(newFrame);
    m_lastLoadedFrame = newFrame;
    if(state == ANIM_WAITING) startTimer();
    emit lastLoadedFrameChanged(newFrame);

}

void Animation::setLastFrame(int newFrame) {
    if(newFrame == m_lastFrame) return;
    m_lastFrame = newFrame;
    emit lastFrameChanged(newFrame);
}

bool Animation::moreFramesExpected() { return false; }

void Animation::lastFrameLoaded() {
    if(state == ANIM_WAITING && frame() == lastFrame()) pause();
}

void Animation::loadingDone() {
    if(state == ANIM_WAITING && m_frame == m_lastLoadedFrame) {
        pause();
    }
}

void Animation::timerEvent(QTimerEvent*) { handleTimerEvent(); }

void Animation::handleTimerEvent() {
    switch(state) {
        case ANIM_STOPPED: {
            return;
        } break;
        case ANIM_RUNNING: {
            if(atLastFrame()) {
                pause();
            }else if(m_frame == m_lastLoadedFrame) {
                state = ANIM_WAITING;
                emit stateChanged();
            }else{
                toNextFrame();
                if(atLastFrame()) {
                    pause();
                }else{
                    startTimer();
                }
            }
        } break;
        case ANIM_WAITING: {
            if(atLastFrame()) {
                pause();
            }else{
                state = ANIM_RUNNING;
                handleTimerEvent();
            }
        } break;
    }
}

void Animation::startTimer() {
    if(!timer.isActive()) {
        timer.start(m_timerValue, this);
        state = ANIM_RUNNING;
    }
}

void Animation::stopTimer() {
    if(timer.isActive()) timer.stop();
    state = ANIM_STOPPED;
}

ProcessAnimation::ProcessAnimation(ProcessPtr process, ProcessOptionsPtr animOptions, QObject* parent):
        Animation(animOptions, parent), m_done(false)
{
    if(process->numIterations() > 0) setLastFrame(process->numIterations());
    if(!QMetaType::type("ProcessResults")) {
        qRegisterMetaType<ProcessResults>("ProcessResults");
    }
    connect(process.data(), SIGNAL(iterationDone(int, QVariantMap)), SLOT(iterationDone(int, QVariantMap)));
    connect(process.data(), SIGNAL(processDone(QVariantMap)), SLOT(processDone()));
    connect(process.data(), SIGNAL(numIterationsChanged(int)), SLOT(numIterationsChanged(int)));
}

void ProcessAnimation::iterationDone(int iterationNumber, QVariantMap results) {
    Q_ASSERT(iterationNumber == m_results.size());
    m_results.append(results);
    setLastLoadedFrame(iterationNumber);
}

void ProcessAnimation::processDone() {
    m_done = true;
    setLastFrame(lastLoadedFrame());
    loadingDone();
}

void ProcessAnimation::numIterationsChanged(int newNumIterations) {
    setLastFrame(newNumIterations);
}

bool ProcessAnimation::moreFramesExpected() {
    return !m_done;
}

QVariantMap ProcessAnimation::resultsOfIteration(int iteration) {
    return m_results.value(iteration);
}
