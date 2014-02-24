#include "ForceFeedbackProcessor.h"
#include "cmath"

FFBWheel::FFBWheel(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<WHEEL_PARAMETER>();
    qRegisterMetaType<VELOCITY_SAMPLE>();
}

qint32 FFBWheel::calculateTorque(TELEMETRY_FEEDBACK telemetry_feedback)
{
    qint32 calculated_torque = telemetry_feedback.torquePct * 32767;

    if      (calculated_torque > 32767)     calculated_torque = 32767;
    else if (calculated_torque < -32768)    calculated_torque = -32768;

//    WHEEL_STATE state = WHEEL_STATE();
//    state.torque = calculated_torque;
//    state.raw_position = raw_position;

    return calculated_torque;
}

