#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include <QBasicTimer>
#include "process_h.h"

/** Stores the state of an animation, and allows modifying the state.
  *
  * An animation is a series of "frames", of which one is always the "current frame" (unless there are no frames).
  * It can be played (in this state, the current frame advances in regular intervals), or paused.
  *
  * New frames can be added to an animation, even while it is playing. When a playing animations reaches
  * the final frame, but more frames are still expected to be added, it pauses and waits for more frames.
  * As soon as these are added, it resumes playing again.
  */
class Animation: public QObject {
    Q_OBJECT
public:
    Animation(ProcessOptionsPtr animOptions, QObject* parent=0);
    static ProcessOptionsPtr newOptions();
public slots:
    // These return true iff the frame is changed to the requested value:
    bool toNextFrame();
    bool toPreviousFrame();
    bool toFirstFrame();
    bool toLastFrame();
    bool setFrame(int);
    // These return true iff the state is changed
    bool play();
    bool pause();
    bool playPause(); // also re-starts animation if on last frame
signals:
    void playbackStarted();
    void playbackEnded();
    void lastLoadedFrameChanged(int);
    void lastFrameChanged(int);
    void frameChanged(int);
    void stateChanged();
public:
    int frame();
    // Waiting for next frame to load is considered playing in this function:
    bool playing();
    bool waiting();
    bool isLoadingDone();
public:
    int lastLoadedFrame();
    int lastFrame();
    bool atLastFrame();
protected:
    void setLastLoadedFrame(int);
    void setLastFrame(int);
    virtual bool moreFramesExpected();
    void lastFrameLoaded();
    void loadingDone();
protected:
    void timerEvent(QTimerEvent*);
    void handleTimerEvent();
private:
    void startTimer();
    void stopTimer();
private:
    enum State {
        ANIM_STOPPED, ///< The animation is stopped.
        ANIM_RUNNING, ///< The animation is currently running
        ANIM_WAITING ///< Waiting for new frame; will switch to ANIM_RUNNING as soon as it loads
    } state;
    int m_frame;
    int m_lastLoadedFrame;
    int m_lastFrame;
    int m_timerValue;
    QBasicTimer timer;
    bool stopAtNextFrame;
};

/** An Animation that displays the intermediate results of a Process. The process' iterations
  * correspond to the animation's frames.
  *
  * The results of the setup of a process are in frame 0. The first "real" iteration has number 1.
  * This numbering is inherited from Process.
  */
class ProcessAnimation: public Animation {
    Q_OBJECT
public:
    ProcessAnimation(ProcessPtr process, ProcessOptionsPtr animOptions, QObject* parent=0);
    QVariantMap resultsOfIteration(int iteration);
private slots:
    void iterationDone(int iterationNumber, QVariantMap results);
    void processDone();
    void numIterationsChanged(int newNumIterations);
protected:
    bool moreFramesExpected();
private:
    bool m_done;
private:
    QList<QVariantMap> m_results;
};

typedef QSharedPointer<Animation> AnimationPtr;
typedef QSharedPointer<ProcessAnimation> ProcessAnimationPtr;

#endif // ANIMATION_H
