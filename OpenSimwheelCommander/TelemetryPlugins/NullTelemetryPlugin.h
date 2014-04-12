#ifndef NULLTELEMETRYPLUGIN_H
#define NULLTELEMETRYPLUGIN_H

#include "TelemetryPluginInterface.h"
#include "QGroupBox"

class NullTelemetryPlugin : public TelemetryPluginInterface
{
    public:
        TelemetryFeedback Update() {
            TelemetryFeedback feedback = TelemetryFeedback();
            feedback.isConnected = false;
            Sleep(500);
            return feedback;
        }

        QWidget* GetSettingsWidget() {
            return new QGroupBox("No Plugin");
        }

        void Shutdown() {

        }
};

#endif // NULLTELEMETRYPLUGIN_H
