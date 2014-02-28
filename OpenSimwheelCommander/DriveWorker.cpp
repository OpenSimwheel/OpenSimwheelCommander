#include "driveworker.h"

#include "qelapsedtimer.h"
#include "simplemotion.h"

#include "windows.h"
#include "qmessagebox.h"

#include "TelemetryWorker.h"
#include "qthread.h"

#include <QSettings>

DriveWorker::DriveWorker(WHEEL_PARAMETER* wheelParameter, TELEMETRY_FEEDBACK* telemetryFeedback, QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<FEEDBACK_DATA>();
    qRegisterMetaType<CALCULATION_RESULT>();
    qRegisterMetaType<TELEMETRY_FEEDBACK>();

    qRegisterMetaType<WheelSettings>();
    qRegisterMetaType<JoystickDeviceInfo>();

    this->WheelParameter = wheelParameter;
    this->TelemetryFeedback = telemetryFeedback;

    this->SmCommunicator = new SimpleMotionCommunicator();
    this->Joystick = new JoystickManager();
}

DriveWorker::~DriveWorker()
{
    shutdown();
}

void DriveWorker::shutdown()
{
    SmCommunicator->SetParameter(SMP_OSW_TORQUE_SETPOINT, 0);
    SmCommunicator->DisableDrive();
    SmCommunicator->Disconnect();
}

void DriveWorker::process()
{
    emit initializing();

    if (!Joystick->Aquire(1))
        return;

    SmCommunicator->Connect(WheelParameter->ComPort, WheelParameter->DeviceAddress);

    //waiting for drive to finish initialization
    SmCommunicator->WaitForDriveReady();

    SmCommunicator->EnableDrive();

#ifdef USE_FAST_COMMAND
    const qint64 LOOP_TIMEOUT = 2000; // In µs
#else
    const qint64 LOOP_TIMEOUT = 4000; // In µs
#endif

    const qint64 WAKEUP_RESOLUTION = 1100; // In µs

    StartCounter();

    SmCommunicator->SetParameter(SMP_FAULTS, 0); //clear faults if any

    qint64 lastLoop = 0;
    qint64 frameStartCounter = 0;

    CALCULATION_RESULT res = CALCULATION_RESULT();
    FEEDBACK_DATA feedback = FEEDBACK_DATA();

    res.torque = 0;

    feedback.position = 10;
    feedback.torque = 10;
    feedback.calculationBenchmark = 0;
    feedback.lastLoopBenchmark = 0;

    pos = findHome();

    smint32 dutyCycle;

    SmCommunicator->SetParameter(SMP_PWM_DUTYCYCLE, 2000);
    SmCommunicator->GetParameter(SMP_PWM_DUTYCYCLE, &dutyCycle);
    feedback.debug1 = dutyCycle;


    UpdateWheelParameter();
    SmCommunicator->InitializeLowLevelCommunication(SMP_OSW_TORQUE_SETPOINT,SMP_OSW_JOYSTICK_POS);

    emit initialized();

    WHEEL_PARAMETER lastWheelParameter = *WheelParameter;

    while(true)
    {
       qint64 counter = GetCounter();

       lastLoop = counter - frameStartCounter;
       frameStartCounter = counter;


       WHEEL_PARAMETER wheelParameter = *WheelParameter;
       TELEMETRY_FEEDBACK telemetryFeedback = *TelemetryFeedback;

       if (!(WheelParameterEqual(wheelParameter, lastWheelParameter)))
       {
            UpdateWheelParameter();
            lastWheelParameter = wheelParameter;
            SmCommunicator->InitializeLowLevelCommunication(SMP_OSW_TORQUE_SETPOINT, SMP_OSW_JOYSTICK_POS);
            continue;
       }

       qint32 torque = ffbwheel.calculateTorque(telemetryFeedback);

       qint64 stamp1 = GetCounter();

// communication BEGIN
       SmCommunicator->AppendCommandToQueue(SMPCMD_24B, torque);
#ifdef USE_FAST_COMMAND
       SmCommunicator->ExecuteFastCommandQueue();
#else
       SmCommunicator->ExecuteCommandQueue();
#endif
       SmCommunicator->GetQueuedReturnValue(&pos);

//       SmCommunicator->ExperimentalFastGetPosition(&pos);

// communication END

       qint64 stamp2 = GetCounter();

       feedback.calculationBenchmark = stamp2 - stamp1;
       feedback.torque = torque;
       feedback.position = pos;
       feedback.lastLoopBenchmark = lastLoop;

       Joystick->UpdateRelativePosition(pos / (wheelParameter.DegreesOfRotation * (10000.0d/360) / 2));

       emit feedback_received(feedback);


       // sleep for the remainder
       qint64 sleep = LOOP_TIMEOUT - WAKEUP_RESOLUTION - (GetCounter() - frameStartCounter);

       if(sleep >= 0)
           Sleep(sleep / 1000);

       while(GetCounter() - frameStartCounter < LOOP_TIMEOUT) { }
    }
}


smint32 DriveWorker::findHome()
{
    SmCommunicator->SetParameter(SMP_TORQUE_LPF_BANDWIDTH, 1);
    SmCommunicator->SwitchControlMode(SimpleMotionCommunicator::ControlModePosition);

    if (this->WheelParameter->CenterOffsetEnabled)
        SmCommunicator->SetParameter(SMP_TRAJ_PLANNER_HOMING_OFFSET, this->WheelParameter->CenterOffset);
    else
        SmCommunicator->SetParameter(SMP_TRAJ_PLANNER_HOMING_OFFSET, 0);

    SmCommunicator->SetParameter(SMP_HOMING_CONTROL, 1);

    smint32 status, center = 0;
    SmCommunicator->GetParameter(SMP_STATUS, &status);

    while (bool(status & STAT_HOMING)) {
        SmCommunicator->GetParameter(SMP_STATUS, &status);
    }

    SmCommunicator->SetParameter(SMP_OSW_JOYSTICK_POS, center);

    Sleep(200);

    SmCommunicator->SwitchControlMode(SimpleMotionCommunicator::ControlModeTorque);
    SmCommunicator->SetParameter(SMP_TORQUE_LPF_BANDWIDTH, 5);

    emit homing_completed(this->WheelParameter->CenterOffset);
    return center;
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
    settings.HardstopsPosition = smint32((WheelParameter->DegreesOfRotation * (10000/360.0d)) / 2.0d);
    settings.DampingStrength = WheelParameter->DampingEnabled ? smint32(WheelParameter->DampingStrength) : 0;
    settings.CenterSpringStrength = WheelParameter->CenterSpringEnabled ? smint32(WheelParameter->CenterSpringStrength) : 0;
    settings.OverallStrength = smint32(WheelParameter->OverallStrength);

    SmCommunicator->LoadSettings(settings);
}


