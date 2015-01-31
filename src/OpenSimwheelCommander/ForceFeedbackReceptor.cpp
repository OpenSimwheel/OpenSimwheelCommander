#include "ForceFeedbackReceptor.h"



ForceFeedbackReceptor::ForceFeedbackReceptor(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<FFBInfo>();
    ffbInfo = FFBInfo();
}

ForceFeedbackReceptor::~ForceFeedbackReceptor()
{

}



static FFBEType FfbEffect = (FFBEType)-1;
    LPCTSTR FfbEffectName[] =
    {L"NONE", L"Constant Force", L"Ramp",  L"Square",  L"Sine",  L"Triangle",  L"Sawtooth Up",\
    L"Sawtooth Down", L"Spring",  L"Damper",  L"Inertia",  L"Friction",  L"Custom Force"};


void ForceFeedbackReceptor::ProcessFFBData(PVOID ffbData) {

    FFB_EFF_CONST effect;


    if (ERROR_SUCCESS == Ffb_h_Eff_Const((FFB_DATA*)ffbData, &effect ))
    {
        if (effect.EffectType == ET_CONST) {

        }
    }


    emit FFBUpdateReceived(ffbInfo);
}

void ForceFeedbackReceptor::Start() {
    ffbInfo.Started = FfbStart(deviceId);

    if (ffbInfo.Started) {
        FfbRegisterGenCB(ForceFeedbackReceptor::FFBCallback_Wrapper, this);
    }
}


void ForceFeedbackReceptor::SetDeviceId(unsigned int deviceId) {
    this->deviceId = deviceId;
}

void CALLBACK ForceFeedbackReceptor::FFBCallback_Wrapper(PVOID data, PVOID objPtr) {
    ForceFeedbackReceptor* mySelf = (ForceFeedbackReceptor*) objPtr;

    mySelf->ProcessFFBData(data);
}

