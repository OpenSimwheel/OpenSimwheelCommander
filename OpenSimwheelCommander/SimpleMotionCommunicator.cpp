#include "SimpleMotionCommunicator.h"

const smint32 SimpleMotionCommunicator::ControlModeTorque = smint32(CM_TORQUE);
const smint32 SimpleMotionCommunicator::ControlModePosition = smint32(CM_POSITION);

SimpleMotionCommunicator::SimpleMotionCommunicator(QObject *parent) :
    QObject(parent)
{
    isConnected = false;
}

bool SimpleMotionCommunicator::Connect(const char* comPort, smint32 deviceAddress) {
    smbus handle = smOpenBus(comPort);

    if (handle >= 0) {
        busHandle = handle;
        isConnected = true;
        SetDeviceAddress(deviceAddress);
        return true;
    }

    return false;
}

void SimpleMotionCommunicator::Disconnect() {
    smCloseBus(busHandle);
}

SM_STATUS SimpleMotionCommunicator::SetParameter(const smint16 parameterId, smint32 value) {
    return smSetParameter(busHandle, deviceAddress, parameterId, value);
}

SM_STATUS SimpleMotionCommunicator::GetParameter(const smint16 parameterId, smint32 *retValue) {
    return smRead1Parameter(busHandle, deviceAddress, parameterId, retValue);
}

SM_STATUS SimpleMotionCommunicator::AppendCommandToQueue(int commandType, smint32 value) {
    return smAppendSMCommandToQueue(busHandle, commandType, value);
}

SM_STATUS SimpleMotionCommunicator::ExecuteCommandQueue() {
    return smExecuteCommandQueue(busHandle, deviceAddress);
}

SM_STATUS SimpleMotionCommunicator::ExecuteFastCommandQueue() {
    return smExecuteFastCommandQueue(busHandle, deviceAddress);
}



SM_STATUS SimpleMotionCommunicator::GetQueuedReturnValue(smint32 *retValue) {
    return smGetQueuedSMCommandReturnValue(busHandle, retValue);
}

void SimpleMotionCommunicator::InitializeLowLevelCommunication(smint32 setParameterId, smint32 returnParameterId) {
    this->AppendCommandToQueue(SMPCMD_SETPARAMADDR, SMP_RETURN_PARAM_LEN); //2b
    this->AppendCommandToQueue(SMPCMD_24B, SMPRET_24B );//3b
    this->AppendCommandToQueue(SMPCMD_SETPARAMADDR, SMP_RETURN_PARAM_ADDR );//2b
    this->AppendCommandToQueue(SMPCMD_24B, returnParameterId );//3b

    this->AppendCommandToQueue(SMPCMD_SETPARAMADDR, setParameterId  );

    this->ExecuteCommandQueue();

    this->SkipQueuedNullReturnValue();
    this->SkipQueuedNullReturnValue();
    this->SkipQueuedNullReturnValue();
    this->SkipQueuedNullReturnValue();
    this->SkipQueuedNullReturnValue();
}

void SimpleMotionCommunicator::SkipQueuedNullReturnValue() {
    smGetQueuedSMCommandReturnValue(busHandle, &nul);
}

void SimpleMotionCommunicator::EnableDrive() {
    this->SetParameter(SMP_CONTROL_BITS1, SMP_CB1_ENABLE);
}

void SimpleMotionCommunicator::DisableDrive() {
    this->SetParameter(SMP_CONTROL_BITS1, 0);
}

SM_STATUS SimpleMotionCommunicator::SwitchControlMode(smint32 controlMode) {
    this->SetParameter(SMP_CONTROL_MODE, controlMode);
}

void SimpleMotionCommunicator::LoadSettings(WheelSettings settings) {
    this->SetParameter(SMP_OSW_HARDSTOPS_POS, settings.HardstopsPosition);
    this->SetParameter(SMP_OSW_EFFECTS_DAMPING_STR, settings.DampingStrength);
    this->SetParameter(SMP_OSW_EFFECTS_CENTERSPRING_STR, settings.CenterSpringStrength);
    this->SetParameter(SMP_OSW_EFFECTS_OVERALL_STR, settings.OverallStrength);
}
