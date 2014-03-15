#ifndef COMMONSTRUCTS_H
#define COMMONSTRUCTS_H

#include <simplemotion_defs.h>
#include <simplemotion.h>
#include <QString>

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
    qint8 DampingStrength; // in pct
    bool DampingEnabled;

    qint8 CenterSpringStrength; // in pct
    bool CenterSpringEnabled;

    qint8 OverallStrength; // in pct

    qint16 DegreesOfRotation; // in degrees
    bool CenterOffsetEnabled;
    qint32 CenterOffset;
} WHEEL_PARAMETER;

Q_DECLARE_METATYPE(WHEEL_PARAMETER)

typedef struct
{
    const char* ComPort;
    qint32 DeviceAddress;
    quint16 CommunicationTimeoutMs;
} OSWDriveParameter;

Q_DECLARE_METATYPE(OSWDriveParameter)

typedef struct
{
    quint8 StartupFrequency;
} OSWOptions;

Q_DECLARE_METATYPE(OSWOptions)

#endif // COMMONSTRUCTS_H
