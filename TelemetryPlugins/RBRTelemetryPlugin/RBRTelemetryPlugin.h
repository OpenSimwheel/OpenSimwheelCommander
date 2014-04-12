#ifndef RBRTELEMETRYPLUGIN_H
#define RBRTELEMETRYPLUGIN_H


#include "TelemetryPlugins\TelemetryPluginInterface.h"
#include <QtCore>
#include <QtDebug>

class RBRTelemetryPlugin : public QObject, TelemetryPluginInterface
{
    Q_INTERFACES(TelemetryPluginInterface)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface" FILE "osw_plugin_rbr.json")

public:
    RBRTelemetryPlugin();
    TelemetryFeedback Update();

private:
    TelemetryFeedback feedback;
};

#endif // RBRTELEMETRYPLUGIN_H
