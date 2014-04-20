#include "rFactorTelemetryPlugin.h"

#include "SettingsGroupBox.h"

#include "QFileInfo"

#define _USE_MATH_DEFINES
#include <cmath>
#include <QGroupBox>
#include <time.h>

static HANDLE hMapFile = NULL;
static const char *pSharedMem = NULL;
static const TelemetryInfo *pTelemetryInfo = NULL;

static bool isInitialized = false;
static int lastTickCount = INT_MAX;
static time_t lastValidTime = 0;

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
        lastTickCount = INT_MAX;
    }

    if (hMapFile) {
        pSharedMem = (const char *)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
        pTelemetryInfo = (TelemetryInfo *)pSharedMem;
        lastTickCount = INT_MAX;
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
    lastTickCount = INT_MAX;

    isInitialized = false;
}


bool rFactorTelemetryPlugin::IsNewDataAvailable()
{
    if (isInitialized || Initialize())
    {
//        // not driving, so no new data neccesary
//        if (!(pTelemetryInfo->InRealtime || pTelemetryInfo->InSession))
//        {
//            lastTickCount = INT_MAX;
//            return false;
//        }

        // this is a newer report, so tell there is new data
        if (lastTickCount < pTelemetryInfo->Tick)
        {
            lastTickCount = pTelemetryInfo->Tick;
            lastValidTime = time(NULL);
            return true;
        }
        // we are in front, probably a new session or a disconnect inbetween?
        else if (lastTickCount > pTelemetryInfo->Tick)
        {
            lastTickCount = pTelemetryInfo->Tick;
            return false;
        }
    }
    return false;
}

bool rFactorTelemetryPlugin::WaitForNewData(int timeout)
{
    if (isInitialized || Initialize())
    {
        if (IsNewDataAvailable())
            return true;

        // wait for new data
        while(!IsNewDataAvailable()) {
            Sleep(0);
        }
        return true;
    }

    // in case of an error, sleep for the timeout
    if (timeout > 0)
        Sleep(timeout);

    return false;
}

TelemetryInfo rFactorTelemetryPlugin::GetNewData()
{
    if (WaitForNewData(16))
    {
        TelemetryInfo info = *pTelemetryInfo;
        return info;
    }
    else //something went wrong, so return a safe value
    {
        TelemetryInfo info = TelemetryInfo();
        info.SteeringArmForce = 0;
        return info;
    }

}

TelemetryFeedback rFactorTelemetryPlugin::Update()
{
    TelemetryInfo info = GetNewData();
    feedback.isConnected = true;

    float force = info.SteeringArmForce;



    float upperLimit = pluginSettings->GetMaxSteeringArmForce();
    float lowerLimit = 0-upperLimit;

    if (force > upperLimit)         force = upperLimit;
    else if (force < lowerLimit)    force = lowerLimit;

    float torquePct = force / upperLimit;

    if (torqueQueue.count() > 1) {
        int weightAvg = pluginSettings->GetSmoothingPct();

        if (info.Speed < 25) { // to compensate for extreme jitter at standstill
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

    feedback.torque = info.SteeringArmForce;

    if (pluginSettings->GetIsInverted())
        feedback.torquePct = feedback.torquePct * -1;

    return feedback;
}

Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface")
