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

    qint32 calculateTorque(TELEMETRY_FEEDBACK telemetry_feedback);

private:
    WHEEL_PARAMETER* wheel_parameter;

signals:

public slots:

};

#endif // FFBWHEEL_H
