#include "ForceFeedbackProcessor.h"
#include "cmath"

FFBWheel::FFBWheel(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<WHEEL_PARAMETER>();
}

qint32 FFBWheel::calculateTorque(TelemetryFeedback telemetry_feedback)
{
    qint32 calculated_torque = telemetry_feedback.torquePct * -16384;

    if      (calculated_torque > 16383)     calculated_torque = 16383;
    else if (calculated_torque < -16384)    calculated_torque = -16384;

    return calculated_torque;
}

