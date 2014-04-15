#include "telemetryworker.h"
#include "windows.h"

TelemetryWorker::TelemetryWorker(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TelemetryFeedback>();
    telemetryPlugin = NULL;
}

void TelemetryWorker::SetPlugin(TelemetryPluginInterface* iTelemetryPlugin) {
    if (telemetryPlugin != NULL)
        telemetryPlugin->Shutdown();

    telemetryPlugin = iTelemetryPlugin;
    telemetryPlugin->Startup();
}

void TelemetryWorker::DeleteCurrentPlugin() {
//    telemetryPlugin->Shutdown();
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
