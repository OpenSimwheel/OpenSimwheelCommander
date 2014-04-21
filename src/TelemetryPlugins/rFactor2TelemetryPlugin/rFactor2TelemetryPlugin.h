#ifndef RFACTOR2TELEMETRYPLUGIN_H
#define RFACTOR2TELEMETRYPLUGIN_H

#include "..\inc\TelemetryPluginInterface.h"
#include <QtCore>
#include <QtDebug>
#include <QQueue>
#include <QSettings>

#include "windows.h"
#include "TelemetryInfo.h"
#include "PluginSettings.h"

static const wchar_t* RFACTOR_OSW_MEMMAPFILENAME     = L"Local\\rFactor2OpenSimwheelMMF";

class rFactor2TelemetryPlugin : public QObject, TelemetryPluginInterface
{
    Q_INTERFACES(TelemetryPluginInterface)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface" FILE "osw_plugin_rfactor2.json")

public:
    rFactor2TelemetryPlugin();
    TelemetryFeedback Update();
    QWidget* GetSettingsWidget();
    void Shutdown();
    void Startup();

private:
    TelemetryFeedback feedback;
    bool Initialize();
    QQueue<float> torqueQueue;
    PluginSettings* pluginSettings;

    bool IsNewDataAvailable();
    TelemetryInfo GetNewData();
    bool WaitForNewData(int timeout);
};

#endif // RFACTOR2TELEMETRYPLUGIN_H
