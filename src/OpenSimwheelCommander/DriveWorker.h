#ifndef DRIVESTAGEMANAGER_H
#define DRIVESTAGEMANAGER_H

#include <QObject>
#include "Libraries/SimpleMotionV2/simplemotion.h"

#include <QQueue>
#include "ForceFeedbackProcessor.h"
#include "TelemetryWorker.h"

#include "TelemetryPlugins/TelemetryPluginInterface.h"

#include "SimpleMotionCommunicator.h"

#include "JoystickManager.h"

class DriveWorker : public QObject
{
    Q_OBJECT
public:
    explicit DriveWorker(OSWDriveParameter* DriveParameter, WHEEL_PARAMETER* WheelParameter, TelemetryFeedback *telemetryFeedback, OSWOptions* options, QObject *parent = 0);
    ~DriveWorker();

    WHEEL_PARAMETER* WheelParameter;
    TelemetryFeedback* TelemetryFeedbackData;
    OSWDriveParameter* DriveParameter;
    OSWOptions* Options;

    JoystickManager* Joystick;

    void UpdateWheelParameter();

    SimpleMotionCommunicator* SmCommunicator;

    void Pause() { run = false; }
    void Start() { run = true; }

private:
    smint32 pos;

    bool run;

    FFBWheel ffbwheel;

    double PCFreq;
    __int64 CounterStart;

    void StartCounter();
    qint64 GetCounter();

    bool WheelParameterEqual(WHEEL_PARAMETER& me, WHEEL_PARAMETER& other)
    {
        return me.DampingStrength == other.DampingStrength &&
               me.DampingEnabled == other.DampingEnabled &&
                me.CenterSpringStrength == other.CenterSpringStrength &&
                me.CenterSpringEnabled == other.CenterSpringEnabled &&
                me.OverallStrength == other.OverallStrength &&
                me.DegreesOfRotation == other.DegreesOfRotation;
    }
signals:
    void initializing();
    void initialized();
    void feedback_received(FEEDBACK_DATA feedback_data);
    void telemetry_updated(TelemetryFeedback TelemetryFeedbackData);
public slots:
    void process();
    void shutdown();
};



#endif // DRIVESTAGEMANAGER_H
