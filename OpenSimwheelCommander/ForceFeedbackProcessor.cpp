#include "ForceFeedbackProcessor.h"
#include "cmath"
#include "qdebug.h"

FFBWheel::FFBWheel(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<WHEEL_PARAMETER>();
    qRegisterMetaType<VELOCITY_SAMPLE>();
}

WHEEL_STATE FFBWheel::process(WHEEL_PARAMETER wheel_parameter, qint32 raw_position, qint64 dt, WHEEL_STATE lastState, TELEMETRY_FEEDBACK telemetry_feedback)
{
    qint32 pos_delta = raw_position - lastState.raw_position;

    if (pos_delta > 32768 )
        pos_delta -= 65536;
    else if (pos_delta < -32768)
        pos_delta += 65536;

    qint32 pos = lastState.position + pos_delta;

    VELOCITY_SAMPLE sample;
    sample.time = dt;
    sample.pos_delta = pos_delta;

    velocitySamplesStack.append(sample);

    int stackSize = velocitySamplesStack.count();

    if (stackSize > velocityNumSamples) {
        velocitySamplesStack.removeFirst();
        --stackSize;
    }

    qint64 sumTime = 0;
    qint32 sumPosDelta = 0;

    for (int i = 0; i < stackSize; i++) {
        VELOCITY_SAMPLE smp = velocitySamplesStack.at(i);

        sumTime += smp.time;
        sumPosDelta += smp.pos_delta;
    }

    double velocity = ((sumPosDelta * 1000000.0d) / sumTime) / 2500.0d;

    qint32 limit = (wheel_parameter.DegreesOfRotation * (10000.d / 360)) / 2;

    if (telemetry_feedback.steeringWheelLock > 100)
        limit = (telemetry_feedback.steeringWheelLock * (10000.d / 360)) / 2;

    qint32 calculated_torque = 0;

    if (pos > limit)
    {
        calculated_torque = getSpringTorque(25.0d, pos, -0.22d, velocity, limit);
    }
    else if (pos < 0-limit)
    {
        calculated_torque = getSpringTorque(25.0d, pos, -0.22d, velocity, 0-limit);
    }
    else
    {
        double b = -0.070;
        qint32 damperTorque = (getDamperTorque(b, velocity) * -1) * (wheel_parameter.DampingStrength / 100.0d)/* * telemetry_feedback.damperPct*/;
        qint32 springTorque = getSpringTorque(0.990d, pos, -0.040d, velocity, 0) * (wheel_parameter.CenterSpringStrength / 100.0d);

        float telemetryTorque = telemetry_feedback.torquePct * 32000;

        calculated_torque = (telemetryTorque + (springTorque - damperTorque));
    }

    calculated_torque =  calculated_torque * (wheel_parameter.OverallStrength / 100.0d);

    if (calculated_torque > 32000)
        calculated_torque = 32000;
    else if (calculated_torque < -32000)
        calculated_torque = -32000;

    WHEEL_STATE state = WHEEL_STATE();
    state.torque = calculated_torque;
    state.position = pos;
    state.velocity = velocity;
    state.raw_position = raw_position;

    lastState = state;

    return state;
}

qint32 FFBWheel::getDamperTorque(double b, double velocity)
{
    return (-b * velocity * 10000.0d) * -1;
}

qint32 FFBWheel::getSpringTorque(double k, qint32 x, double b, double velocity, qint32 offset)
{
    return (qint32)((k * (x - offset) - (b * velocity * 10000.0f)) * -1);
}


