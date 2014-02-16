#ifndef DRIVESTAGEMANAGER_H
#define DRIVESTAGEMANAGER_H

#include <QObject>
#include <simplemotion.h>
#include <libs/vJoySDK/src/stdafx.h>
#include <libs/vJoySDK/inc/public.h>
#include <libs/vJoySDK/inc/vjoyinterface.h>

#include <libs/irsdk/yaml_parser.h>
#include <libs/irsdk/irsdk_defines.h>

#include <QQueue>
#include <ForceFeedbackProcessor.h>
#include <TelemetryWorker.h>

class DriveWorker : public QObject
{
    Q_OBJECT
public:
    explicit DriveWorker(WHEEL_PARAMETER* wheel_parameter, TELEMETRY_FEEDBACK *telemetryFeedback, QObject *parent = 0);
    ~DriveWorker();
    smint32 deviceAddress;
    smbus busHandle;


    WHEEL_PARAMETER* wheel_parameter;
    TELEMETRY_FEEDBACK* telemetry_feedback;

private:
    VjdStat vjoy_device_status;

    smint32 pos = 0;

    FFBWheel ffbwheel;

    void initSmLowLevel();

    double PCFreq = 0.0;
    __int64 CounterStart = 0;

    void StartCounter();
    qint64 GetCounter();

    smint32 findHome();

signals:
    void initializing();
    void initialized();
    void feedback_received(FEEDBACK_DATA feedback_data);
    void homing_completed(qint32 center);
    void telemetry_updated(TELEMETRY_FEEDBACK telemetry_feedback);
public slots:
    void process();
    void shutdown();
};



#endif // DRIVESTAGEMANAGER_H
