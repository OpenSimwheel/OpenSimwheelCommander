#ifndef SIMPLEMOTIONCOMMUNICATOR_H
#define SIMPLEMOTIONCOMMUNICATOR_H

#include <QObject>
#include "Libraries/SimpleMotionV2/simplemotion_defs.h"
#include "Libraries/SimpleMotionV2/simplemotion.h"

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

    bool Connect(QString comPort, smint32 deviceAddress, quint16 connectionTimeoutMs);
    void Disconnect();

    bool IsConnected() {
        return isConnected;
    }

    bool IsReady() {
        return isReady;
    }

    void ClearFaults() {
        this->SetParameter(SMP_FAULTS, 0); //clear faults if any
    }

    SM_STATUS SetParameter(const smint16 parameterId, smint32 value);
    SM_STATUS GetParameter(const smint16 parameterId, smint32* retValue);
    SM_STATUS AppendCommandToQueue(int commandType, smint32 value);
    SM_STATUS ExecuteCommandQueue();
    SM_STATUS ExecuteFastCommandQueue();
    void SkipQueuedNullReturnValue();
    SM_STATUS GetQueuedReturnValue(smint32* retValue);

    void InitializeLowLevelCommunication(smint32 setParameterId, smint32 returnParameterId);

    void AutoCalibrate(smint32 homingFrequency, smint32 centerOffset);

    void Shutdown() {
        this->SetTorque(0);
        this->DisableDrive();
        this->Disconnect();
    }

    void EnableDrive();
    void DisableDrive();



    void SetPwmDutyCycle(smint32 dutyCycle) {
        this->SetParameter(SMP_PWM_DUTYCYCLE, dutyCycle);
    }

    void SetTorqueBandwithLimit(smint32 torqueBandwidthLimit) {
        this->SetParameter(SMP_TORQUE_LPF_BANDWIDTH, torqueBandwidthLimit);
    }

    void SetTorque(smint32 torque) {
        this->SetParameter(SMP_OSW_TORQUE_SETPOINT, torque);
    }

    //blocking call
    void WaitForDriveReady() {
        smint32 status;
        this->GetParameter(SMP_STATUS, &status);

        while (bool(status & STAT_INITIALIZED) == false) {
            this->GetParameter(SMP_STATUS, &status);
        }

        isReady = true;
    }

    void LoadSettings(WheelSettings settings);

    SM_STATUS SwitchControlMode(smint32 controlMode);
private:
    smint32 deviceAddress;
    smbus busHandle;
    bool isConnected;
    bool isReady;

    smint32 nul;


signals:

public slots:

};

#endif // SIMPLEMOTIONCOMMUNICATOR_H
