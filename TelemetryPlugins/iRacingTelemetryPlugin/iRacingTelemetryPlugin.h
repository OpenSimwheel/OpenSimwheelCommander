#ifndef IRACINGTELEMETRYPLUGIN_H
#define IRACINGTELEMETRYPLUGIN_H

#include "TelemetryPlugins\TelemetryPluginInterface.h"
#include <QtCore>
#include <QtDebug>
#include <Settings/SettingsGroupBox.h>
#include <QQueue>
#include <QSettings>

class iRacingTelemetryPlugin : public QObject, TelemetryPluginInterface
{
    Q_INTERFACES(TelemetryPluginInterface)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface" FILE "osw_plugin_iracing.json")

public:
    iRacingTelemetryPlugin();
    TelemetryFeedback Update();
    QWidget* GetSettingsWidget();
    void Shutdown();

private:
    char *g_data = NULL;
    int g_nData;
    int g_timeout;
    int g_steeringwheel_torque_offset;
    int g_steeringwheel_angle_offset;
    int g_steeringwheel_torquepct_offset;
    int g_steeringwheel_torquepctstops_offset;
    int g_steeringwheel_anglemax_offset;
    int g_steeringwheel_damperpct_offset;

    QQueue<float> torqueQueue;
    QSettings SavedSettings;

    TelemetryFeedback feedback;
    PluginSettings* settings;
    SettingsGroupBox* settingsGroupBox;
};

#endif // IRACINGTELEMETRYPLUGIN_H
