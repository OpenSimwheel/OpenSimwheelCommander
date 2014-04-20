#include "driveworker.h"

#include "simplemotion.h"

#include "windows.h"
#include "qmessagebox.h"

#include "TelemetryWorker.h"
#include "qthread.h"

#include <QSettings>

DriveWorker::DriveWorker(OSWDriveParameter* driveParameter,
                         WHEEL_PARAMETER* wheelParameter,
                         TelemetryFeedback* telemetryFeedback,
                         OSWOptions* options,
                         QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<FEEDBACK_DATA>();
    qRegisterMetaType<TelemetryFeedback>();
    qRegisterMetaType<OSWDriveParameter>();
    qRegisterMetaType<OSWOptions>();

    qRegisterMetaType<WheelSettings>();
    qRegisterMetaType<JoystickDeviceInfo>();

    this->WheelParameter = wheelParameter;
    this->TelemetryFeedbackData = telemetryFeedback;
    this->Options = options;

    this->SmCommunicator = new SimpleMotionCommunicator();
    this->Joystick = new JoystickManager();

    this->DriveParameter = driveParameter;

    pos = 0;
    run = false;
}

DriveWorker::~DriveWorker()
{
    SmCommunicator->Shutdown();
}

void DriveWorker::shutdown()
{
    SmCommunicator->Shutdown();
}

void DriveWorker::process()
{

    qint64 LOOP_TIMEOUT = 4000; // In µs
    OSWDriveParameter driveParameter = *DriveParameter;
    if (driveParameter.UseFastCommunication) {
        LOOP_TIMEOUT = 2000; // In µs
    }


    qint64 lastLoop = 0, frameStartCounter = 0;
    FEEDBACK_DATA feedback = FEEDBACK_DATA();
    WHEEL_PARAMETER lastWheelParameter = *WheelParameter;


    emit initializing();

    if (!Joystick->Aquire(1))
        return;

    emit initialized();

    StartCounter();
    while(true)
    {
       while(!run) {
           Sleep(100);
       }

       qint64 counter = GetCounter();

       lastLoop = counter - frameStartCounter;
       frameStartCounter = counter;

       WHEEL_PARAMETER wheelParameter = *WheelParameter;
       TelemetryFeedback telemetryFeedback = *TelemetryFeedbackData;



       qint32 torque = ffbwheel.calculateTorque(telemetryFeedback);

// communication BEGIN
       qint64 stamp1 = GetCounter();
       SmCommunicator->AppendCommandToQueue(SMPCMD_24B, torque);

       if (driveParameter.UseFastCommunication) {
           SmCommunicator->ExecuteFastCommandQueue();
       } else {
           SmCommunicator->ExecuteCommandQueue();
       }

       SmCommunicator->GetQueuedReturnValue(&pos);
       qint64 stamp2 = GetCounter();
// communication END

//       Joystick->UpdateRelativePosition(pos / (wheelParameter.DegreesOfRotation * (10000.0d/360) / 2));
       Joystick->UpdatePosition(pos);

       feedback.calculationBenchmark = stamp2 - stamp1;
       feedback.torque = torque;
       feedback.position = pos;
       feedback.lastLoopBenchmark = lastLoop;

       emit feedback_received(feedback);

       if (!(WheelParameterEqual(wheelParameter, lastWheelParameter)))
       {
            UpdateWheelParameter();
            lastWheelParameter = wheelParameter;
       }

       while(GetCounter() - frameStartCounter < LOOP_TIMEOUT) { }
    }
}



void DriveWorker::StartCounter() {
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
        qDebug("QueryPerformanceFrequency failed!\n");

    PCFreq = double(li.QuadPart) / 1000000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

qint64 DriveWorker::GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);

    return qint64(li.QuadPart-CounterStart) / PCFreq;
}

void DriveWorker::UpdateWheelParameter()
{
    WheelSettings settings = WheelSettings();
    settings.HardstopsPosition = smint32(WheelParameter->DegreesOfRotation);
    settings.DampingStrength = WheelParameter->DampingEnabled ? smint32(WheelParameter->DampingStrength) : 0;
    settings.CenterSpringStrength = WheelParameter->CenterSpringEnabled ? smint32(WheelParameter->CenterSpringStrength) : 0;
    settings.OverallStrength = smint32(WheelParameter->OverallStrength);

    SmCommunicator->LoadSettings(settings);
    SmCommunicator->InitializeLowLevelCommunication(SMP_OSW_TORQUE_SETPOINT, SMP_OSW_JOYSTICK_POS);
}


