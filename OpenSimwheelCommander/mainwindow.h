#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <driveworker.h>
#include <telemetryworker.h>
#include <QQueue>
#include <QSettings>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProgressDialog>
#include <Splash/IndestructableSplashScreen.h>
#include <JoystickManager.h>

#include <QStringList>
#include <QDir>

#ifdef USE_FAST_COMMAND
    #define MAX_LATENCY 2100
#else
    #define MAX_LATENCY 4100
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QSettings* ApplicationSettings;


    ~MainWindow();

public slots:
    void updateFeedbackInfo(FEEDBACK_DATA data);
    void updateTelemetryFeedbackInfo(TelemetryFeedback feedback);

    void onWheelInitializing();
    void onWheelInitialized();
    void setSplashScreen(IndestructableSplashScreen* splash);

    void onJoystickInitialized(JoystickDriverInfo driverInfo, JoystickDeviceInfo deviceInfo);
    void onJoystickPositionUpdated(qint32 pos);
protected:
    void closeEvent(QCloseEvent *);

private slots:
    void on_action_Quit_triggered();

    void on_action_Drive_Stage_triggered();

    void on_actionWheel_Settings_triggered();

    void on_action_About_triggered();

    void on_action_Joystick_Settings_triggered();

    void on_action_Restart_triggered();

    void on_action_Plugins_triggered();

    void on_action_Options_triggered();

private:
    Ui::MainWindow *ui;

    qint64 count;
    qint64 avg;
    qint64 min;
    qint64 max;
    qint64 sum;

    WHEEL_PARAMETER WheelParameter;
    TelemetryFeedback telemetry_feedback;
    OSWDriveParameter driveParameter;
    OSWOptions options;

    QThread* telemetryThread;
    TelemetryWorker* telemetryWorker;

    DriveWorker* driveWorker;
    QThread* driveThread;

    QStringList pluginFileNames;
    QDir pluginsDir;

    void SaveLayout();
    void RestoreLayout();

    void LoadPlugins();

    IndestructableSplashScreen *splash;
signals:
};


#endif // MAINWINDOW_H
