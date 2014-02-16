#ifndef TELEMETRYWORKER_H
#define TELEMETRYWORKER_H

#include <QObject>
#include <CommonStructs.h>

class TelemetryWorker : public QObject
{
    Q_OBJECT
public:
    explicit TelemetryWorker(QObject *parent = 0);


private:
    char *g_data = NULL;
    int g_nData;
    int g_timeout;
    int g_steeringwheel_torque_offset;
    int g_steeringwheel_angle_offset;
    int g_steeringwheel_torquepct_offset;
    int g_steeringwheel_anglemax_offset;
    int g_steeringwheel_damperpct_offset;

    void irsdk_process();
signals:
    void telemetry_feedback_received(TELEMETRY_FEEDBACK telemetryFeedback);
public slots:
    void process();
};

#endif // TELEMETRYWORKER_H
