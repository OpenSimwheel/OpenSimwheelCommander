#ifndef SIMPLEMOTIONCOMMUNICATOR_H
#define SIMPLEMOTIONCOMMUNICATOR_H

#include <QObject>
#include <simplemotion_defs.h>
#include <simplemotion.h>

typedef struct
{
    smint32 HardstopsPosition;
    smint32 OverallStrength;
    smint32 DampingStrength;
    smint32 CenterSpringStrength;
} WheelSettings;

Q_DECLARE_METATYPE(WheelSettings)

class SimpleMotionCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit SimpleMotionCommunicator(QObject *parent = 0);

    static const smint32 ControlModeTorque;
    static const smint32 ControlModePosition;

    void SetDeviceAddress(smint32 deviceAddress) {
        this->deviceAddress = deviceAddress;
    }

    smint32 GetDeviceAddress() {
        return this->deviceAddress;
    }

    smbus GetBusHandle() {
        return this->busHandle;
    }

    bool Connect(const char* comPort, smint32 deviceAddress);
    void Disconnect();

    bool IsConnected() {
        return isConnected;
    }

    SM_STATUS SetParameter(const smint16 parameterId, smint32 value);
    SM_STATUS GetParameter(const smint16 parameterId, smint32* retValue);
    SM_STATUS AppendCommandToQueue(int commandType, smint32 value);
    SM_STATUS ExecuteCommandQueue();
    SM_STATUS ExecuteFastCommandQueue();
    void SkipQueuedNullReturnValue();
    SM_STATUS GetQueuedReturnValue(smint32* retValue);

    void InitializeLowLevelCommunication(smint32 setParameterId, smint32 returnParameterId);

    void EnableDrive();
    void DisableDrive();

    void LoadSettings(WheelSettings settings);

    SM_STATUS SwitchControlMode(smint32 controlMode);
private:
    smint32 deviceAddress;
    smbus busHandle;
    bool isConnected;

    smint32 nul;


signals:

public slots:

};

#endif // SIMPLEMOTIONCOMMUNICATOR_H
