#ifndef TELEMETRYINFO_H
#define TELEMETRYINFO_H

#include <windows.h>

struct TelemetryInfo
{
    float Gear;
    float EngineRPM;
    float EngineMaxRPM;

    float Speed;

    float GLongitudinal;
    float GLateral;
    float GVertical;



    float Pitch;
    float Roll;
    float Yaw;

    float Brake;
    float Throttle;
    float Clutch;

    float Steering;
    float SteeringArmForce;
    float ForceFeedbackValue;

    bool InSession;
    bool InRealtime;

    char TrackName[64];
    char VehicleName[64];

    unsigned int Tick;
} static MMF;

#endif
