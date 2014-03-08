#include "JoystickManager.h"

JoystickManager::JoystickManager(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<JoystickDriverInfo>();
}

JoystickManager::~JoystickManager()
{
    RelinquishVJD(DeviceId);
}

bool JoystickManager::Aquire(unsigned int deviceId) {
    if (vJoyEnabled()) {
        DeviceId = UINT(deviceId);
        DeviceStatus = GetVJDStatus(UINT(DeviceId));

        GetVJDAxisMax(DeviceId,HID_USAGE_X,&AxisMaxValue);
        GetVJDAxisMin(DeviceId,HID_USAGE_X,&AxisMinValue);

        report.bDevice = (BYTE)DeviceId;
    }

    if ((DeviceStatus == VJD_STAT_OWN) || ((DeviceStatus == VJD_STAT_FREE) && (!AcquireVJD(DeviceId)))) {
        isAquired = false;
    } else {
        isAquired = true;

        JoystickDriverInfo driverInfo = JoystickDriverInfo();
        driverInfo.Enabled = true;
        driverInfo.Vendor = QString::fromWCharArray((const wchar_t*)GetvJoyManufacturerString());
        driverInfo.Product = QString::fromWCharArray((const wchar_t*)(GetvJoyProductString()));
        driverInfo.VersionNumber = QString::fromWCharArray((const wchar_t*)(GetvJoySerialNumberString()));

        JoystickDeviceInfo deviceInfo = JoystickDeviceInfo();
        deviceInfo.Id = deviceId;
        deviceInfo.AxisMin = AxisMinValue;
        deviceInfo.AxisMax = AxisMaxValue;
        deviceInfo.Status = GetHumanReadableStatus(GetVJDStatus(UINT(DeviceId)));

        emit Initialized(driverInfo, deviceInfo);
    }




    return isAquired;
}

void JoystickManager::UpdateRelativePosition(double posPct) {
    if (!isAquired) return;

    if (posPct > 1)  posPct = 1;
    if (posPct < -1) posPct = -1;

//    qint32 rangeHalf = (AxisMaxValue - AxisMinValue) / 2;

//    qint32 pos = (qint32)((posPct * rangeHalf) + rangeHalf);
    qint32 pos = (qint32(posPct *16384 ) + 16384);

    report.wAxisX = pos;
    UpdateVJD(DeviceId, &report);

    emit PositionUpdated(pos);
}

void JoystickManager::Center() {
    this->UpdateRelativePosition(0);
}

QString JoystickManager::GetHumanReadableStatus(VjdStat status) {
    switch (status)
    {
        case VJD_STAT_OWN:
            return "aquired";
        case VJD_STAT_FREE:
            return "free";
        case VJD_STAT_BUSY:
            return "already owned by another feeder (busy)";
        case VJD_STAT_MISS:
            return "not installed or disabled";
        default:
            return "undefined error";
    };
}
