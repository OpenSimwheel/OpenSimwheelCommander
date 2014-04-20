#ifndef TELEMETRYINFO_H
#define TELEMETRYINFO_H

#include <windows.h>

struct TelemetryInfo
{
    bool Running;
    int Tick;
    float ForceValue;
} static MMF;

#endif
