#ifndef RFACTORTELEMETRYPLUGIN_H
#define RFACTORTELEMETRYPLUGIN_H

#include "TelemetryPlugins\TelemetryPluginInterface.h"
#include <QtCore>
#include <QtDebug>
#include <QQueue>
#include <QSettings>

#include "windows.h"
#include "TelemetryInfo.h"
#include "PluginSettings.h"

static const wchar_t* RFACTOR_OSW_MEMMAPFILENAME     = L"Local\\rFactorOpenSimwheelMMF";

class rFactorTelemetryPlugin : public QObject, TelemetryPluginInterface
{
    Q_INTERFACES(TelemetryPluginInterface)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface" FILE "osw_plugin_rfactor.json")

public:
    rFactorTelemetryPlugin();
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
