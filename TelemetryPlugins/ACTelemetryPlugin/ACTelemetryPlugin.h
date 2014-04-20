#ifndef ACTELEMETRYPLUGIN_H
#define ACTELEMETRYPLUGIN_H

#include "TelemetryPlugins\TelemetryPluginInterface.h"
#include <QtCore>
#include <QtDebug>
#include <QQueue>
#include <QSettings>

#include "windows.h"
#include "TelemetryInfo.h"
#include "PluginSettings.h"

static const wchar_t* MEMMAPFILENAME     = L"Local\\AssettoCorsaOpenSimwheel";

class ACTelemetryPlugin : public QObject, TelemetryPluginInterface
{
    Q_INTERFACES(TelemetryPluginInterface)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface" FILE "osw_plugin_ac.json")

public:
    ACTelemetryPlugin();
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

#endif // ACTELEMETRYPLUGIN_H
