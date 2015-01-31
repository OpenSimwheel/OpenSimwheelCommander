#ifndef JOYSTICKMANAGER_H
#define JOYSTICKMANAGER_H

#include <QObject>


#include <vjoyincludes.h>
#include <ForceFeedbackReceptor.h>

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
    void ProcessFFBData(PVOID ffbData);
    void StartFFB();
    FFBInfo GetForceFeedbackInfo();

private:
    VjdStat DeviceStatus;

    LONG AxisMinValue;
    LONG AxisMaxValue;

    UINT DeviceId;

    JOYSTICK_POSITION report;

    bool isAquired;

    ForceFeedbackReceptor* ffbReceptor;


    QString GetHumanReadableStatus(VjdStat status);
signals:
    void Initialized(JoystickDriverInfo driverInfo, JoystickDeviceInfo deviceInfo);
    void PositionUpdated(qint32 position);
public slots:
    void onForceFeedbackUpdateReceived(FFBInfo ffbInfo);
};

#endif // JOYSTICKMANAGER_H
