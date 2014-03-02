#include "telemetryworker.h"
#include "windows.h"

TelemetryWorker::TelemetryWorker(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TelemetryFeedback>();
}

void TelemetryWorker::process()
{
    while (true)
    {
        if (telemetryPlugin) {
            TelemetryFeedback feedback = telemetryPlugin->Update();
            emit telemetry_feedback_received(feedback);
        }
        Sleep(0);
    }
}
