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
    void updateTelemetryFeedbackInfo(TELEMETRY_FEEDBACK feedback);
    void onHomingCompleted(qint32 center);

    void onWheelInitializing();
    void onWheelInitialized();
    void setSplashScreen(IndestructableSplashScreen* splash);

private slots:
    void on_action_Quit_triggered();

    void on_action_Drive_Stage_triggered();

    void on_actionWheel_Settings_triggered();

    void on_action_About_triggered();

    void on_action_Joystick_Settings_triggered();

    void on_action_Restart_triggered();

private:
    Ui::MainWindow *ui;

    qint64 count;
    qint64 avg;
    qint64 min;
    qint64 max;
    qint64 sum;

    WHEEL_PARAMETER WheelParameter;
    TELEMETRY_FEEDBACK telemetry_feedback;

    QThread* telemetryThread;
    TelemetryWorker* telemetryWorker;

    DriveWorker* driveWorker;
    QThread* driveThread;


    IndestructableSplashScreen *splash;
signals:
};


#endif // MAINWINDOW_H
