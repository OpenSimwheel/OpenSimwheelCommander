#include "TestTelemetryPlugin.h"

#include "SettingsGroupBox.h"

#include "QFileInfo"

#define _USE_MATH_DEFINES
#include <cmath>
#include <QGroupBox>

static HANDLE hMapFile = NULL;
static const char *pSharedMem = NULL;
static const TelemetryInfo *pTelemetryInfo = NULL;

static bool isInitialized = false;
static int lastTickCount = INT_MAX;


TestTelemetryPlugin::TestTelemetryPlugin() {
    feedback = TelemetryFeedback();

    torqueQueue = QQueue<float>();
    pluginSettings = new PluginSettings();
}

QWidget* TestTelemetryPlugin::GetSettingsWidget() {
    QWidget* widget = new QGroupBox("Test Plugin");
    return widget;
}

void TestTelemetryPlugin::Startup() {
    if (!isInitialized)
        Initialize();
}

bool TestTelemetryPlugin::Initialize() {
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

void TestTelemetryPlugin::Shutdown() {
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


static double maxFF = 0;
static double minFF = 0;

TelemetryFeedback TestTelemetryPlugin::Update()
{
    if (isInitialized || Initialize()) {
        int currentTickCount = pTelemetryInfo->Tick;

        feedback.isConnected = pTelemetryInfo->Running;

            float forceValue = pTelemetryInfo->ForceValue;

            if (forceValue > maxFF) {
                maxFF = forceValue;
                feedback.debug1 = QString("%1 | %2").arg(minFF).arg(maxFF);
            } else if (forceValue < minFF) {
                minFF = forceValue;
                feedback.debug1 = QString("%1 | %2").arg(minFF).arg(maxFF);
            }

            feedback.torque = (pTelemetryInfo->Steer);
            feedback.torquePct = forceValue / 3;

        Sleep(3);
        return feedback;

    } else { // not driving, so don't turn the wheel ;)
        //TelemetryFeedback fb = TelemetryFeedback();
        feedback.torquePct = 0;
        feedback.debug1 = "inactive";
        feedback.isConnected = false;
        Sleep(3); // we don't need to stress the processor unneccesarily
        return feedback;
    }


}

Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface")
