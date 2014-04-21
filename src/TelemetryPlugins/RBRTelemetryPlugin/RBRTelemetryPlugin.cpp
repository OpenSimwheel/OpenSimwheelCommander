#include "RBRTelemetryPlugin.h"
#include "windows.h"

#define _USE_MATH_DEFINES
#include <cmath>

RBRTelemetryPlugin::RBRTelemetryPlugin() {
    feedback = TelemetryFeedback();
}

TelemetryFeedback RBRTelemetryPlugin::Update()
{
    float ffb = 0;

    for (;;) {
//        char* pApp = (char*) 0x007EAC48;




        char* pApp = (char*) 0x18F6E4;
        char* app = (char*) *pApp;
        char* ffboffest = app + (0xBC0 + 0x3C + 0x08);



//        ffb = *(float*) app;

        if (ffb != 0) {
            feedback.isConnected = true;
            feedback.damperPct = ffb;
        } else {
            feedback.isConnected = false;
        }
        Sleep(0);
    }

    return feedback;
}

Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface")
