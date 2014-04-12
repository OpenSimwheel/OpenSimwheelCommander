#include "iRacingTelemetryPlugin.h"

#include "irsdk/irsdk_defines.h"
#include "irsdk/yaml_parser.h"

#include "windows.h"

#include "Settings/SettingsGroupBox.h"
#include "QFileInfo"

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
    settings = new PluginSettings();

//    settingsGroupBox = new SettingsGroupBox(settings);

    torqueQueue = QQueue<float>();

}

QWidget* iRacingTelemetryPlugin::GetSettingsWidget() {
    QWidget* widget = new SettingsGroupBox(settings);
    return widget;
}

void iRacingTelemetryPlugin::Shutdown() {

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

                g_steeringwheel_torquepct_offset = irsdk_varNameToOffset("SteeringWheelPctTorqueSign");
                g_steeringwheel_torquepctstops_offset = irsdk_varNameToOffset("SteeringWheelPctTorqueSignStops");

                g_steeringwheel_damperpct_offset = irsdk_varNameToOffset("SteeringWheelPctDamper");
                g_steeringwheel_anglemax_offset = irsdk_varNameToOffset("SteeringWheelAngleMax");

                const char *valstr;
                int valstrlen;
                char str[512];

                int carIdx = -1;
                char pathStr[512];

                // get the playerCarIdx
                if(parseYaml(irsdk_getSessionInfoStr(), "DriverInfo:DriverCarIdx:", &valstr, &valstrlen))
                    carIdx = atoi(valstr);

                if(carIdx >= 0)
                {
                    // get drivers car path
                    sprintf(str, "DriverInfo:Drivers:CarIdx:{%d}CarPath:", carIdx);
                    if(parseYaml(irsdk_getSessionInfoStr(), str, &valstr, &valstrlen))
                    {
                        strncpy(pathStr, valstr, valstrlen);
                        pathStr[valstrlen] = '\0';
                    }
                }

                feedback.debug1 = QString(pathStr);
                settings->CarName = pathStr;
            }
            else if(g_data)
            {
//                PluginSettings activeSettings = *settings;
                float torquePct = 0;

                feedback.torque = * ((float *)(g_data + g_steeringwheel_torque_offset));

                if (settings->GetUseHardstops())
                    torquePct = * ((float *)(g_data + g_steeringwheel_torquepctstops_offset));
                else
                    torquePct = * ((float *)(g_data + g_steeringwheel_torquepct_offset));

                feedback.angle = * ((float *)(g_data + g_steeringwheel_angle_offset));
                feedback.damperPct = * ((float *)(g_data + g_steeringwheel_damperpct_offset));
                feedback.steeringWheelLock = * ((float *)(g_data + g_steeringwheel_anglemax_offset));

                feedback.angle = feedback.angle * 180.0 / M_PI;
                feedback.steeringWheelLock = feedback.steeringWheelLock * 180.0 / M_PI;

                if (torqueQueue.count() > 1) {
                    int weightAvg = settings->GetSmoothingPct();
                    int weightCur = 100 - weightAvg;


                    float avgTorque = 0;
                    float sumTorque = 0;


                    int bufferSize = torqueQueue.count();

                    for (int i = 0; i < bufferSize; i++) {
                        sumTorque += torqueQueue.value(i);
                    }

                    float weightedSumTorque = sumTorque * weightAvg;
                    float weightedCurTorque = torquePct * weightCur;

                    avgTorque = (weightedSumTorque + weightedCurTorque) / (weightAvg * bufferSize + weightCur);

                    if (bufferSize >= 5) {
                        torqueQueue.takeFirst();
                    }

                    torqueQueue.append(torquePct);

                    feedback.torquePct = avgTorque;
                } else {
                    torqueQueue.append(torquePct);
                    feedback.torquePct = torquePct;
                }

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
