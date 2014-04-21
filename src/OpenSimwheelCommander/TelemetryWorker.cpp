#include "telemetryworker.h"
#include "windows.h"

TelemetryWorker::TelemetryWorker(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TelemetryFeedback>();
    telemetryPlugin = NULL;
    PCFreq = 0.0;
    CounterStart = 0;

}

TelemetryWorker::~TelemetryWorker()
{
    this->Shutdown();
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

void TelemetryWorker::Shutdown()
{
    TelemetryFeedback feedback = TelemetryFeedback();
    emit telemetry_feedback_received(feedback);
}

void TelemetryWorker::process()
{
    const qint64 LOOP_TIMEOUT = 2000; //in us, eq. 500Hz update rate
    qint64 lastLoop = 0;
    qint64 frameStartCounter = 0;
    TelemetryFeedback feedback = TelemetryFeedback();

    StartCounter();

    while(true)
    {
        qint64 counter = GetCounter();
        lastLoop = counter - frameStartCounter;
        frameStartCounter = counter;


        if (telemetryPlugin) {
            feedback = telemetryPlugin->Update();
        }

        feedback.deltaT = lastLoop;
        emit telemetry_feedback_received(feedback);

        while(GetCounter() - frameStartCounter < LOOP_TIMEOUT) {
            Sleep(0); // fee at least some ressources
        }
    }
}



void TelemetryWorker::StartCounter() {
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
        qDebug("QueryPerformanceFrequency failed!\n");

    PCFreq = double(li.QuadPart) / 1000000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

qint64 TelemetryWorker::GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);

    return qint64(li.QuadPart-CounterStart) / PCFreq;
}
