#include "rFactorTelemetryPlugin.h"

#include "SettingsGroupBox.h"

#include "QFileInfo"

#define _USE_MATH_DEFINES
#include <cmath>
#include <QGroupBox>

static HANDLE hMapFile = NULL;
static const char *pSharedMem = NULL;
static const TelemetryInfo *pTelemetryInfo = NULL;

static bool isInitialized = false;
static int lastTickCount = UINT_MAX;


rFactorTelemetryPlugin::rFactorTelemetryPlugin() {
    feedback = TelemetryFeedback();

    torqueQueue = QQueue<float>();
    pluginSettings = new PluginSettings();
}

QWidget* rFactorTelemetryPlugin::GetSettingsWidget() {
    QWidget* widget = new SettingsGroupBox(pluginSettings);
    return widget;
}

void rFactorTelemetryPlugin::Startup() {
    if (!isInitialized)
        Initialize();
}

bool rFactorTelemetryPlugin::Initialize() {
    if (!hMapFile) {
        hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, RFACTOR_OSW_MEMMAPFILENAME);
        lastTickCount = UINT_MAX;
    }

    if (hMapFile) {
        pSharedMem = (const char *)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
        pTelemetryInfo = (TelemetryInfo *)pSharedMem;
        lastTickCount = UINT_MAX;
    }

    if (pSharedMem) {
        lastTickCount = pTelemetryInfo->Tick;
        isInitialized = true;
        return isInitialized;
    }

    isInitialized = false;
    return isInitialized;
}

void rFactorTelemetryPlugin::Shutdown() {
    if (pSharedMem)
        UnmapViewOfFile(pSharedMem);

    if (hMapFile)
        CloseHandle(hMapFile);

    pSharedMem = NULL;
    pTelemetryInfo = NULL;
    hMapFile = NULL;
    lastTickCount = UINT_MAX;

    isInitialized = false;
}



TelemetryFeedback rFactorTelemetryPlugin::Update()
{


    if (isInitialized || Initialize()) {

//        print(QString::number(sizeof(pTelemetryInfo) + "\n"));

        unsigned int currentTickCount = pTelemetryInfo->Tick;

//        printf("currentTickCount: %u - lastTickCount: %u", currentTickCount, lastTickCount);


        feedback.isConnected = true;

        if (lastTickCount < currentTickCount) { // seems to be newer data

            bool inRealtime = pTelemetryInfo->InRealtime;
            bool inSession = pTelemetryInfo->InSession;
            lastTickCount = currentTickCount;
            feedback.debug1 = QString::number(pTelemetryInfo->ForceFeedbackValue);
//            feedback.debug1 = "In Realtime: %s\nIn Session: %s", inRealtime ? "yes" : "no", inSession ? "yes" : "no");

            if (inRealtime && inSession) {
                float force = pTelemetryInfo->SteeringArmForce * -1;

                float upperLimit = pluginSettings->GetMaxSteeringArmForce();
                float lowerLimit = 0-upperLimit;

                if (force > upperLimit)         force = upperLimit;
                else if (force < lowerLimit)    force = lowerLimit;

                float torquePct = force / upperLimit;

                if (torqueQueue.count() > 1) {
                    int weightAvg = pluginSettings->GetSmoothingPct();

                    if (pTelemetryInfo->Speed < 25) { // to compensate for extreme jitter at standstill
                        weightAvg = 100;
                    }


                    int weightCur = 100 - weightAvg;


                    float avgTorque = 0;
                    float sumTorque = 0;


                    int bufferSize = torqueQueue.count();

                    for (int i = 0; i < bufferSize; i++) {
                        sumTorque += torqueQueue.value(i);
                    }

                    float weightedSumTorque = sumTorque * weightAvg;
                    float weightedCurTorque = torquePct * weightCur;

                    avgTorque = (weightedSumTorque + weightedCurTorque) / (weightAvg * bufferSize + weightCur);

                    if (bufferSize >= 20) {
                        torqueQueue.takeFirst();
                    }

                    torqueQueue.append(torquePct);

                    feedback.torquePct = avgTorque;
                } else {
                    torqueQueue.append(torquePct);
                    feedback.torquePct = torquePct;
                }

                feedback.torque = pTelemetryInfo->SteeringArmForce;

            } else { // not driving, so don't turn the wheel ;)
                TelemetryFeedback fb = TelemetryFeedback();
                fb.torquePct = 0;
                fb.debug1 = "inactive";
                fb.isConnected = true;
                Sleep(0); // we don't need to stress the processor unneccesarily
                return fb;
            }
        }

    } else {
        feedback.isConnected = false;
    }

    Sleep(1); // just to slow things down a little for dev..

    return feedback;
}

Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface")
