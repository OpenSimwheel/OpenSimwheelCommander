#ifndef TELEMETRYPLUGININTERFACE_H
#define TELEMETRYPLUGININTERFACE_H

#include <QtPlugin>

typedef struct
{
    float torque;
    float torquePct;
    float damperPct;
    float steeringWheelLock;
    float angle;
    bool isConnected;
} TelemetryFeedback;

Q_DECLARE_METATYPE(TelemetryFeedback)

class QStringList;
class QString;

class TelemetryPluginInterface
{
    public:
        virtual ~TelemetryPluginInterface() {}

        virtual TelemetryFeedback Update() = 0;
};

#define TelemetryPluginInterface_iid "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface"

Q_DECLARE_INTERFACE(TelemetryPluginInterface, TelemetryPluginInterface_iid)

#endif // TELEMETRYPLUGININTERFACE_H
