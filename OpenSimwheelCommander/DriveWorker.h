#ifndef DRIVESTAGEMANAGER_H
#define DRIVESTAGEMANAGER_H

#include <QObject>
#include <simplemotion.h>

#include <libs/irsdk/yaml_parser.h>
#include <libs/irsdk/irsdk_defines.h>

#include <QQueue>
#include <ForceFeedbackProcessor.h>
#include <TelemetryWorker.h>

#include <TelemetryPlugins/TelemetryPluginInterface.h>

#include <SimpleMotionCommunicator.h>

#include <JoystickManager.h>

#define USE_FAST_COMMAND

class DriveWorker : public QObject
{
    Q_OBJECT
public:
    explicit DriveWorker(WHEEL_PARAMETER* WheelParameter, TelemetryFeedback *telemetryFeedback, QObject *parent = 0);
    ~DriveWorker();

    WHEEL_PARAMETER* WheelParameter;
    TelemetryFeedback* TelemetryFeedbackData;

    JoystickManager* Joystick;

    void UpdateWheelParamter();


private:


    smint32 pos = 0;

    FFBWheel ffbwheel;
    SimpleMotionCommunicator* SmCommunicator;


    double PCFreq = 0.0;
    __int64 CounterStart = 0;

    void StartCounter();
    qint64 GetCounter();

    smint32 findHome();

    void UpdateWheelParameter();

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
    void homing_completed(qint32 center);
    void telemetry_updated(TelemetryFeedback TelemetryFeedbackData);
public slots:
    void process();
    void shutdown();
};



#endif // DRIVESTAGEMANAGER_H
