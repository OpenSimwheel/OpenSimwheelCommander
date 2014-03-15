#include "ForceFeedbackProcessor.h"
#include "cmath"

FFBWheel::FFBWheel(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<WHEEL_PARAMETER>();
}

qint32 FFBWheel::calculateTorque(TelemetryFeedback telemetry_feedback)
{
    qint32 calculated_torque = telemetry_feedback.torquePct * (-32767);

    if      (calculated_torque > 32767)     calculated_torque = 32767;
    else if (calculated_torque < -32768)    calculated_torque = -32768;

    return calculated_torque;
}

