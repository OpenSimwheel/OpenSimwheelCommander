#ifndef FORCEFEEDBACKRECEPTOR_H
#define FORCEFEEDBACKRECEPTOR_H

#include <QObject>



#include <vjoyincludes.h>
#include "CommonStructs.h"

class ForceFeedbackReceptor : public QObject
{
    Q_OBJECT

public:
    explicit ForceFeedbackReceptor(QObject *parent = 0);
    ~ForceFeedbackReceptor();

    static void CALLBACK FFBCallback_Wrapper(PVOID obj, PVOID data);

    void SetDeviceId(unsigned int deviceId);
    void Start();
    void ProcessFFBData(PVOID data);
    void Poll();
    FFBInfo GetForceFeedbackInfo();

private:
    FFBInfo ffbInfo;
    unsigned int deviceId;


signals:
    void FFBUpdateReceived(FFBInfo ffbInfo);
};

#endif // FORCEFEEDBACKRECEPTOR_H
