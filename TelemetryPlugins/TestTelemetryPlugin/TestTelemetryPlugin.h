#ifndef TESTTELEMETRYPLUGIN_H
#define TESTTELEMETRYPLUGIN_H

#include "TelemetryPlugins\TelemetryPluginInterface.h"
#include <QtCore>
#include <QtDebug>
#include <QQueue>
#include <QSettings>

#include "windows.h"
#include "TelemetryInfo.h"
#include "PluginSettings.h"

static const wchar_t* MEMMAPFILENAME     = L"Local\\AssettoCorsaOpenSimwheel";

class TestTelemetryPlugin : public QObject, TelemetryPluginInterface
{
    Q_INTERFACES(TelemetryPluginInterface)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface" FILE "osw_plugin_test.json")

public:
    TestTelemetryPlugin();
    TelemetryFeedback Update();
    QWidget* GetSettingsWidget();
    void Shutdown();
    void Startup();

private:
    TelemetryFeedback feedback;
    bool Initialize();
    QQueue<float> torqueQueue;
    PluginSettings* pluginSettings;
};

#endif // RFACTORTELEMETRYPLUGIN_H
