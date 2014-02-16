#ifndef FFBWHEEL_H
#define FFBWHEEL_H

#include <QObject>
#include <TelemetryWorker.h>
#include <QStack>


class FFBWheel : public QObject
{
    Q_OBJECT
public:
    explicit FFBWheel(QObject *parent = 0);

    QVector<VELOCITY_SAMPLE> velocitySamplesStack;
    const int velocityNumSamples = 1;

    qint32 getDamperTorque(double b, double velocity);
    qint32 getSpringTorque(double k, qint32 x, double b, double velocity, qint32 offset);

    WHEEL_STATE process(WHEEL_PARAMETER wheel_parameter, qint32 pos, qint64 dt, WHEEL_STATE lastState, TELEMETRY_FEEDBACK telemetry_feedback);

private:
    WHEEL_PARAMETER* wheel_parameter;






signals:

public slots:

};

#endif // FFBWHEEL_H
