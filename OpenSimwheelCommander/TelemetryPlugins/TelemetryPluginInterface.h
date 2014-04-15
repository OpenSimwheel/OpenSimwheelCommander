#ifndef TELEMETRYPLUGININTERFACE_H
#define TELEMETRYPLUGININTERFACE_H

#include <QtPlugin>
#include <QVariant>

#include <QWidget>

typedef struct
{
    float torque;
    float torquePct;
    float damperPct;
    float steeringWheelLock;
    float angle;
    bool isConnected;
    QString debug1;
} TelemetryFeedback;

Q_DECLARE_METATYPE(TelemetryFeedback)

class QStringList;
class QString;

class TelemetryPluginInterface
{
    public:
        virtual ~TelemetryPluginInterface() {}

        virtual TelemetryFeedback Update() = 0;
        virtual QWidget* GetSettingsWidget() = 0;
        virtual void Shutdown() = 0;
        virtual void Startup() = 0;
};

#define TelemetryPluginInterface_iid "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface"

Q_DECLARE_INTERFACE(TelemetryPluginInterface, TelemetryPluginInterface_iid)

#endif // TELEMETRYPLUGININTERFACE_H
