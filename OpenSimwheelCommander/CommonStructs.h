#ifndef COMMONSTRUCTS_H
#define COMMONSTRUCTS_H

#include <simplemotion_defs.h>
#include <simplemotion.h>
#include <QString>

typedef struct
{
    float torque;
    float torquePct;
    float damperPct;
    float steeringWheelLock;
    float angle;
} TELEMETRY_FEEDBACK;

Q_DECLARE_METATYPE(TELEMETRY_FEEDBACK)

typedef struct
{
    smint32 position;
    smint32 torque;
    qint64 calculationBenchmark;
    qint64 lastLoopBenchmark;
    smint32 debug1;
    smint32 debug2;
} FEEDBACK_DATA;

Q_DECLARE_METATYPE(FEEDBACK_DATA)

typedef struct
{
    qint32 torque;
} CALCULATION_RESULT;

Q_DECLARE_METATYPE(CALCULATION_RESULT)


typedef struct {
    qint32 pos_delta;
    qint64 time;
} VELOCITY_SAMPLE;

Q_DECLARE_METATYPE(VELOCITY_SAMPLE)

typedef struct
{
    qint8 DampingStrength; // in pct
    bool DampingEnabled;

    qint8 CenterSpringStrength; // in pct
    bool CenterSpringEnabled;

    qint8 OverallStrength; // in pct

    qint16 DegreesOfRotation; // in degrees
    bool CenterOffsetEnabled;
    qint32 CenterOffset;

    const char* ComPort;
    qint32 DeviceAddress;
} WHEEL_PARAMETER;

Q_DECLARE_METATYPE(WHEEL_PARAMETER)

#endif // COMMONSTRUCTS_H
