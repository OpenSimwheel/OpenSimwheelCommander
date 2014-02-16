#include "driveworker.h"

#include "qelapsedtimer.h"
#include "simplemotion.h"

#include "windows.h"
#include "qmessagebox.h"

#include "TelemetryWorker.h"
#include "qthread.h"

DriveWorker::DriveWorker(WHEEL_PARAMETER* wheelParameter, TELEMETRY_FEEDBACK* telemetryFeedback, QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<FEEDBACK_DATA>();
    qRegisterMetaType<CALCULATION_RESULT>();
    qRegisterMetaType<TELEMETRY_FEEDBACK>();

    this->wheel_parameter = wheelParameter;
    this->telemetry_feedback = telemetryFeedback;
}

DriveWorker::~DriveWorker()
{
    shutdown();
}

void DriveWorker::shutdown()
{
    smSetParameter(busHandle, deviceAddress, SMP_ABSOLUTE_POS_TARGET, 0);
    smSetParameter(busHandle,deviceAddress,SMP_CONTROL_BITS1, 0);
    smCloseBus(busHandle);
}

void DriveWorker::process()
{
    emit initializing();

    /** aquire vJoy begin **/

    UINT vjoy_device_id = 1;
    LONG axis_max, axis_min;

    if (vJoyEnabled()) {
        vjoy_device_status = GetVJDStatus(vjoy_device_id);

        GetVJDAxisMax(vjoy_device_id,HID_USAGE_X,&axis_max);
        GetVJDAxisMin(vjoy_device_id,HID_USAGE_X,&axis_min);
    }

    if ((vjoy_device_status == VJD_STAT_OWN) || ((vjoy_device_status == VJD_STAT_FREE) && (!AcquireVJD(vjoy_device_id))))
        return;


    /** aquire vJoy end **/

    smSetTimeout(0);

    busHandle=smOpenBus("COM12");
    if(busHandle>=0)
        deviceAddress=1;

    smSetParameter(busHandle,deviceAddress,SMP_CONTROL_BITS1,SMP_CB1_ENABLE);

    const qint64 LOOP_TIMEOUT = 4000; // In µs
    const qint64 WAKEUP_RESOLUTION = 2000; // In µs

    StartCounter();

    smSetParameter(busHandle, deviceAddress, SMP_FAULTS, 0); //clear faults if any

    qint64 lastLoop = 0;
    qint64 frameStartCounter = 0;

    CALCULATION_RESULT res = CALCULATION_RESULT();
    FEEDBACK_DATA feedback = FEEDBACK_DATA();


    JOYSTICK_POSITION report = JOYSTICK_POSITION();
    report.bDevice = (BYTE)vjoy_device_id;

    res.torque = 0;

    feedback.position = 10;
    feedback.torque = 10;
    feedback.velocity = 10;
    feedback.calculationBenchmark = 0;
    feedback.lastLoopBenchmark = 0;


    WHEEL_STATE lastState = WHEEL_STATE();

    smint32 center = findHome();
    lastState.raw_position = center;

    smint32 dutyCycle;
    smSetParameter(busHandle, deviceAddress, SMP_PWM_DUTYCYCLE, 2000);
    smRead1Parameter(busHandle, deviceAddress, SMP_PWM_DUTYCYCLE, &dutyCycle);
    feedback.debug1 = dutyCycle;



    initSmLowLevel();

    emit initialized();

    while(true)
    {
        qint64 counter = GetCounter();

        lastLoop = counter - frameStartCounter;
        frameStartCounter = counter;

       WHEEL_PARAMETER wheelParameter = *wheel_parameter;
       TELEMETRY_FEEDBACK telemetryFeedback = *telemetry_feedback;


       smAppendSMCommandToQueue( busHandle, SMPCMD_24B, lastState.torque);
       smExecuteCommandQueue( busHandle, 1 );
       smGetQueuedSMCommandReturnValue( busHandle, &pos);

       qint64 stamp2 = GetCounter();

       feedback.calculationBenchmark = stamp2 - frameStartCounter;

       feedback.torque = lastState.torque;
       feedback.velocity = lastState.velocity;
       feedback.position = lastState.raw_position;
       feedback.calculatedPosition = lastState.position;

       feedback.lastLoopBenchmark = lastLoop;

       double calculated_pos = lastState.position / (wheelParameter.DegreesOfRotation * (10000.0d/360) / 2);

       if (calculated_pos > 1)
           calculated_pos = 1;
       if (calculated_pos < -1)
           calculated_pos = -1;

       report.wAxisX = (qint32)((calculated_pos * 16384) + 16384);
       UpdateVJD(vjoy_device_id, &report);


       WHEEL_STATE state = ffbwheel.process(wheelParameter, pos, lastLoop, lastState, telemetryFeedback );
       lastState = state;

       emit feedback_received(feedback);

       // </work>

//        If there is enough time in the frame, then sleep for a portion of it to be processor friendly
       qint64 sleep = LOOP_TIMEOUT - WAKEUP_RESOLUTION - (GetCounter() - frameStartCounter);

       if(sleep >= 0)
           Sleep(sleep / 1000);

       while(GetCounter() - frameStartCounter < LOOP_TIMEOUT) { }
    }
}


void DriveWorker::initSmLowLevel()
{
    smint32 nul;

    smAppendSMCommandToQueue( busHandle, SMPCMD_SETPARAMADDR, SMP_RETURN_PARAM_LEN ); //2b
    smAppendSMCommandToQueue( busHandle, SMPCMD_24B, SMPRET_24B );//3b
    smAppendSMCommandToQueue( busHandle, SMPCMD_SETPARAMADDR, SMP_RETURN_PARAM_ADDR );//2b
    smAppendSMCommandToQueue( busHandle, SMPCMD_24B, SMP_ACTUAL_POSITION_FB );//3b

    smAppendSMCommandToQueue( busHandle, SMPCMD_SETPARAMADDR, SMP_ABSOLUTE_POS_TARGET );

    smExecuteCommandQueue( busHandle, deviceAddress );

    smGetQueuedSMCommandReturnValue( busHandle, &nul );
    smGetQueuedSMCommandReturnValue( busHandle, &nul );
    smGetQueuedSMCommandReturnValue( busHandle, &nul );
    smGetQueuedSMCommandReturnValue( busHandle, &nul );
    smGetQueuedSMCommandReturnValue( busHandle, &nul );
}

smint32 DriveWorker::findHome()
{

    smSetParameter(busHandle, deviceAddress, SMP_TORQUE_LPF_BANDWIDTH, 0);
    smSetParameter(busHandle, deviceAddress, SMP_CONTROL_MODE, CM_POSITION);

    if (this->wheel_parameter->CenterOffsetEnabled)
        smSetParameter(busHandle, deviceAddress, SMP_TRAJ_PLANNER_HOMING_OFFSET, this->wheel_parameter->CenterOffset);
    else
        smSetParameter(busHandle, deviceAddress, SMP_TRAJ_PLANNER_HOMING_OFFSET, 0);

    smSetParameter(busHandle,deviceAddress,SMP_HOMING_CONTROL, 1);

    smint32 status, center;
    smRead1Parameter(busHandle, deviceAddress, SMP_STATUS, &status);

    while (bool(status & STAT_HOMING)) {
        smRead1Parameter(busHandle, deviceAddress, SMP_STATUS, &status);
    }
//    smSetParameter(busHandle, deviceAddress, SMP_OSW_JOYSTICK_POS, 0);

    smRead1Parameter(busHandle, deviceAddress, SMP_ACTUAL_POSITION_FB, &center);

    Sleep(200);

    smSetParameter(busHandle, deviceAddress, SMP_CONTROL_MODE, CM_TORQUE);
    smSetParameter(busHandle, deviceAddress, SMP_TORQUE_LPF_BANDWIDTH, 9);

    emit homing_completed(center);
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
