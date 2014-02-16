#include "telemetryworker.h"
#include "libs/irsdk/irsdk_defines.h"
#include "libs/irsdk/yaml_parser.h"

#include "windows.h"

#define _USE_MATH_DEFINES
#include <cmath>

TelemetryWorker::TelemetryWorker(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TELEMETRY_FEEDBACK>();

    g_nData = 0;
    g_timeout = 1;
    g_steeringwheel_torque_offset = 0;
    g_steeringwheel_angle_offset = 0;
    g_steeringwheel_torquepct_offset = 0;
    g_steeringwheel_anglemax_offset = 0;
    g_steeringwheel_damperpct_offset = 0;
}

void TelemetryWorker::process()
{
    while (true)
    {
        irsdk_process();
        Sleep(0);
    }
}


void TelemetryWorker::irsdk_process()
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
                g_steeringwheel_damperpct_offset = irsdk_varNameToOffset("SteeringWheelPctDamper");
                g_steeringwheel_anglemax_offset = irsdk_varNameToOffset("SteeringWheelAngleMax");
            }
            else if(g_data)
            {
                TELEMETRY_FEEDBACK feedback = TELEMETRY_FEEDBACK();

                feedback.torque = * ((float *)(g_data + g_steeringwheel_torque_offset));
                feedback.torquePct = * ((float *)(g_data + g_steeringwheel_torquepct_offset));
                feedback.angle = * ((float *)(g_data + g_steeringwheel_angle_offset));
                feedback.damperPct = * ((float *)(g_data + g_steeringwheel_damperpct_offset));
                feedback.steeringWheelLock = * ((float *)(g_data + g_steeringwheel_anglemax_offset));

                feedback.angle = feedback.angle * 180.0 / M_PI;
                feedback.steeringWheelLock = feedback.steeringWheelLock * 180.0 / M_PI;

                emit telemetry_feedback_received(feedback);
            }
        }
    }
}
