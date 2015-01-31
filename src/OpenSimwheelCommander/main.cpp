#include "mainwindow.h"
#include <QApplication>
#include "SplashScreenManager.h"
#include "Splash/IndestructableSplashScreen.h"
#include "Version.h"
#include <QtPlugin>

#define RESTART_CODE 1000

bool splashFilter(QObject* target, QEvent *event) { return true; }


int main(int argc, char *argv[])
{
    int exit_code;

    do {
        QApplication* app;
        MainWindow* mainWindow;
        SplashScreenManager* splashScreenManager = new SplashScreenManager();

        app = new QApplication (argc, argv);

        QCoreApplication::setOrganizationName("OpenSimwheel");
        QCoreApplication::setOrganizationDomain("opensimwheel.net");
        QCoreApplication::setApplicationName("OpenSimwheel Commander");

        QPixmap splashImg(":/splash/logo-full-light.png");
        splashScreenManager->createSplashScreen(splashImg);


        splashScreenManager->showMessage("Starting OpenSimwheel Commander...");
        mainWindow = new MainWindow(splashScreenManager);

        exit_code = app->exec();

    } while (exit_code == 1000);

    return exit_code;
}


