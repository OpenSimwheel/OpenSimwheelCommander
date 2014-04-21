#include "ACTelemetryPlugin.h"

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


ACTelemetryPlugin::ACTelemetryPlugin() {
    feedback = TelemetryFeedback();

    torqueQueue = QQueue<float>();
    pluginSettings = new PluginSettings();
}

QWidget* ACTelemetryPlugin::GetSettingsWidget() {
    QWidget* widget = new QGroupBox("Assetto Corsa Plugin");
    return widget;
}

void ACTelemetryPlugin::Startup() {
    if (!isInitialized)
        Initialize();
}

bool ACTelemetryPlugin::Initialize() {
    if (!hMapFile) {
        hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, MEMMAPFILENAME);
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

void ACTelemetryPlugin::Shutdown() {
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


bool ACTelemetryPlugin::IsNewDataAvailable()
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

bool ACTelemetryPlugin::WaitForNewData(int timeout)
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

TelemetryInfo ACTelemetryPlugin::GetNewData()
{
    if (WaitForNewData(16))
    {
        TelemetryInfo info = *pTelemetryInfo;
        return info;
    }
    else //something went wrong, so return a safe value
    {
        TelemetryInfo info = TelemetryInfo();
        info.ForceValue = 0;
        info.Running = false;
        return info;
    }
}


TelemetryFeedback ACTelemetryPlugin::Update()
{
    TelemetryInfo info = GetNewData();
    feedback.isConnected = true;

    float force = info.ForceValue;

    if (pluginSettings->GetIsInverted())
        force = force * -1;

    feedback.torquePct = force / 3;

    return feedback;

}

Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface")
