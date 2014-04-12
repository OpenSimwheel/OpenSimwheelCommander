#ifndef JOYSTICKMANAGER_H
#define JOYSTICKMANAGER_H

#include <QObject>

#include <libs/vJoySDK/src/stdafx.h>
#include <libs/vJoySDK/inc/public.h>
#include <libs/vJoySDK/inc/vjoyinterface.h>

typedef struct
{
    bool Enabled;
    QString Vendor;
    QString Product;
    QString VersionNumber;
} JoystickDriverInfo;

Q_DECLARE_METATYPE(JoystickDriverInfo)

typedef struct
{
    unsigned int Id;
    QString Status;
    qint32 AxisMin;
    qint32 AxisMax;
} JoystickDeviceInfo;

Q_DECLARE_METATYPE(JoystickDeviceInfo)

class JoystickManager : public QObject
{
    Q_OBJECT
public:
    explicit JoystickManager(QObject *parent = 0);
    ~JoystickManager();

    bool Aquire(unsigned int deviceId);
    void UpdateRelativePosition(double posPct);
    void UpdatePosition(qint32 pos);
    void Center();
private:
    VjdStat DeviceStatus;

    LONG AxisMinValue;
    LONG AxisMaxValue;

    UINT DeviceId;

    JOYSTICK_POSITION report = JOYSTICK_POSITION();

    bool isAquired;

    QString GetHumanReadableStatus(VjdStat status);
signals:
    void Initialized(JoystickDriverInfo driverInfo, JoystickDeviceInfo deviceInfo);
    void PositionUpdated(qint32 position);
public slots:

};

#endif // JOYSTICKMANAGER_H
