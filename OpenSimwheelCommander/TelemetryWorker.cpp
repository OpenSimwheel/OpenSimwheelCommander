#include "telemetryworker.h"
#include "windows.h"

TelemetryWorker::TelemetryWorker(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TelemetryFeedback>();
    telemetryPlugin = NULL;
}

void TelemetryWorker::SetPlugin(TelemetryPluginInterface* iTelemetryPlugin) {
    telemetryPlugin = iTelemetryPlugin;
}

void TelemetryWorker::DeleteCurrentPlugin() {
    telemetryPlugin = NULL;
}

void TelemetryWorker::process()
{
    do
    {
        if (telemetryPlugin) {
            TelemetryFeedback feedback = telemetryPlugin->Update();
            emit telemetry_feedback_received(feedback);

        }
        Sleep(0);
    } while(true);
}
