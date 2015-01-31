#ifndef SPLASHSCREENMANAGER_H
#define SPLASHSCREENMANAGER_H

#include "Splash/IndestructableSplashScreen.h"
#include <QObject>
#include <QString>

class SplashScreenManager : public QObject
{
    Q_OBJECT
public:
    explicit SplashScreenManager(QObject *parent = 0);
    ~SplashScreenManager();
    void setSplashScreen(IndestructableSplashScreen* splash);

    void createSplashScreen(QPixmap image);

    void closeSplashScreen();

    void showMessage(QString message);

private:
    IndestructableSplashScreen *splash;

signals:

public slots:
};

#endif // SPLASHSCREENMANAGER_H
