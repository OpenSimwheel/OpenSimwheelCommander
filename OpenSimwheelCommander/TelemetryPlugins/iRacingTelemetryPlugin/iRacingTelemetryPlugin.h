#ifndef IRACINGTELEMETRYPLUGIN_H
#define IRACINGTELEMETRYPLUGIN_H

#include "TelemetryPlugins\TelemetryPluginInterface.h"
#include <QtCore>
#include <QtDebug>

class iRacingTelemetryPlugin : public QObject, TelemetryPluginInterface
{
    Q_INTERFACES(TelemetryPluginInterface)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface" FILE "osw_plugin_iracing.json")

public:
    iRacingTelemetryPlugin();
    QString GetName() const;
    QString GetVersion() const;

    TelemetryFeedback Update();

private:
    char *g_data = NULL;
    int g_nData;
    int g_timeout;
    int g_steeringwheel_torque_offset;
    int g_steeringwheel_angle_offset;
    int g_steeringwheel_torquepct_offset;
    int g_steeringwheel_anglemax_offset;
    int g_steeringwheel_damperpct_offset;

    TelemetryFeedback feedback;
};

#endif // IRACINGTELEMETRYPLUGIN_H
