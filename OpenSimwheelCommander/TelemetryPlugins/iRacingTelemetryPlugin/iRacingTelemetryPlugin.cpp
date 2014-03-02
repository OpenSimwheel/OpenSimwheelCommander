#include "iRacingTelemetryPlugin.h"

#include "irsdk/irsdk_defines.h"
#include "irsdk/yaml_parser.h"

#include "windows.h"

#define _USE_MATH_DEFINES
#include <cmath>

iRacingTelemetryPlugin::iRacingTelemetryPlugin() {
    g_nData = 0;
    g_timeout = 20;
    g_steeringwheel_torque_offset = 0;
    g_steeringwheel_angle_offset = 0;
    g_steeringwheel_torquepct_offset = 0;
    g_steeringwheel_anglemax_offset = 0;
    g_steeringwheel_damperpct_offset = 0;

    feedback = TelemetryFeedback();
}

QString iRacingTelemetryPlugin::GetName() const {
    return "iRacing Telemetry Plugin";
}

QString iRacingTelemetryPlugin::GetVersion() const {
    return "0.1.7";
}


TelemetryFeedback iRacingTelemetryPlugin::Update()
{
    if(irsdk_waitForDataReady(g_timeout, g_data))
    {
        const irsdk_header *pHeader = irsdk_getHeader();
        if(pHeader)
        {
            // if header changes size, assume a new connection
            if(!g_data || g_nData != pHeader->bufLen)
            {
                // free old memory, if found
                if(g_data)
                    delete [] g_data;

                g_nData = pHeader->bufLen;
                g_data = new char[g_nData];

                g_steeringwheel_torque_offset = irsdk_varNameToOffset("SteeringWheelTorque");
                g_steeringwheel_angle_offset = irsdk_varNameToOffset("SteeringWheelAngle");
//                g_steeringwheel_torquepct_offset = irsdk_varNameToOffset("SteeringWheelPctTorqueSign");
                g_steeringwheel_torquepct_offset = irsdk_varNameToOffset("SteeringWheelPctTorqueSignStops");
                g_steeringwheel_damperpct_offset = irsdk_varNameToOffset("SteeringWheelPctDamper");
                g_steeringwheel_anglemax_offset = irsdk_varNameToOffset("SteeringWheelAngleMax");
            }
            else if(g_data)
            {
//                TelemetryFeedback feedback = TelemetryFeedback();

                feedback.torque = * ((float *)(g_data + g_steeringwheel_torque_offset));
                feedback.torquePct = * ((float *)(g_data + g_steeringwheel_torquepct_offset));
                feedback.angle = * ((float *)(g_data + g_steeringwheel_angle_offset));
                feedback.damperPct = * ((float *)(g_data + g_steeringwheel_damperpct_offset));
                feedback.steeringWheelLock = * ((float *)(g_data + g_steeringwheel_anglemax_offset));

                feedback.angle = feedback.angle * 180.0 / M_PI;
                feedback.steeringWheelLock = feedback.steeringWheelLock * 180.0 / M_PI;

                feedback.isConnected = true;

                return feedback;
            }
        }
    } else { // iRacing did go away
        TelemetryFeedback nullFeedback = TelemetryFeedback();
        nullFeedback.isConnected = false;
        return nullFeedback;
    }

    return feedback;
}

Q_PLUGIN_METADATA(IID "net.opensimwheel.OpenSimwheelCommander.TelemetryPlugins.TelemetryPluginInterface")
