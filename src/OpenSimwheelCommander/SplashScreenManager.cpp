#include "SplashScreenManager.h"

SplashScreenManager::SplashScreenManager(QObject *parent) : QObject(parent)
{

}

SplashScreenManager::~SplashScreenManager()
{

}

void SplashScreenManager::setSplashScreen(IndestructableSplashScreen* splash)
{
    this->splash = splash;
}

void SplashScreenManager::createSplashScreen(QPixmap image)
{
    this->splash = new IndestructableSplashScreen(image);
}

void SplashScreenManager::closeSplashScreen()
{
    if (this->splash != NULL) {
        this->splash->close();
    }
}

void SplashScreenManager::showMessage(QString message) {
    if (this->splash != NULL) {
        this->splash->showMessage(message, Qt::AlignRight | Qt::AlignBottom);
    }
}
