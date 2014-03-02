#ifndef TELEMETRYWORKER_H
#define TELEMETRYWORKER_H

#include <QObject>
#include <TelemetryPlugins/TelemetryPluginInterface.h>

class TelemetryWorker : public QObject
{
    Q_OBJECT
public:
    explicit TelemetryWorker(QObject *parent = 0);

    void SetPlugin(TelemetryPluginInterface* iTelemetryPlugin) {
        telemetryPlugin = iTelemetryPlugin;
    }

private:
    TelemetryPluginInterface *telemetryPlugin;


signals:
    void telemetry_feedback_received(TelemetryFeedback telemetryFeedback);

public slots:
    void process();
};

#endif // TELEMETRYWORKER_H
