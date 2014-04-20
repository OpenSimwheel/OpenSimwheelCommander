#ifndef TELEMETRYWORKER_H
#define TELEMETRYWORKER_H

#define STAT_RUNNING 1
#define STAT_STOPPED 0

#include <QObject>
#include <TelemetryPlugins/TelemetryPluginInterface.h>
#include <QMutex>

class TelemetryWorker : public QObject
{
    Q_OBJECT
public:
    explicit TelemetryWorker(QObject *parent = 0);
    ~TelemetryWorker();

    void SetPlugin(TelemetryPluginInterface* iTelemetryPlugin);
    void DeleteCurrentPlugin();
private:
    TelemetryPluginInterface* telemetryPlugin;
    void StartCounter();
    qint64 GetCounter();

    double PCFreq = 0.0;
    __int64 CounterStart = 0;
signals:
    void telemetry_feedback_received(TelemetryFeedback telemetryFeedback);

public slots:
    void process();
    void Shutdown();
};

#endif // TELEMETRYWORKER_H
