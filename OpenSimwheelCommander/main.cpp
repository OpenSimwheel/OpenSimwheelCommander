#include "mainwindow.h"
#include <QApplication>
#include <Splash/IndestructableSplashScreen.h>
#include <Version.h>

#define RESTART_CODE 1000

bool splashFilter(QObject* target, QEvent *event) { return true; }

int main(int argc, char *argv[])
{
    int exit_code;

    do {
        QApplication* app;
        MainWindow* mainWindow;


        app = new QApplication (argc, argv);

        QCoreApplication::setOrganizationName("OpenSimwheel");
        QCoreApplication::setOrganizationDomain("opensimwheel.net");
        QCoreApplication::setApplicationName("OpenSimwheel Commander");

        QPixmap splashImg(":/splash/OpenSimwheel.png");
        IndestructableSplashScreen *splash = new IndestructableSplashScreen(splashImg);

        splash->show();

        splash->showMessage("Starting OpenSimwheel Commander...", Qt::AlignBottom);


        mainWindow = new MainWindow();
        mainWindow->setSplashScreen(splash);

        exit_code = app->exec();

    } while (exit_code == 1000);

    return exit_code;
}


