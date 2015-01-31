#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DriveWorker.h"
#include "TelemetryWorker.h"
#include <QQueue>
#include <QSettings>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QProgressDialog>

#include "TelemetryPlugins/NullTelemetryPlugin.h"
#include "JoystickManager.h"

#include "SplashScreenManager.h"

#include <QStringList>
#include <QDir>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SplashScreenManager *splashScreenManager, QWidget *parent = 0);
    QSettings* ApplicationSettings;




    ~MainWindow();

public slots:
    void updateFeedbackInfo(FEEDBACK_DATA data);
    void updateTelemetryFeedbackInfo(TelemetryFeedback feedback);


    void onJoystickInitialized(JoystickDriverInfo driverInfo, JoystickDeviceInfo deviceInfo);
    void onJoystickPositionUpdated(qint32 pos);
    void onFFBUpdateReceived(FFBInfo ffbInfo);
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

    void on_comboBox_plugins_currentIndexChanged(int index);

    void on_btnStartDriveWorker_clicked();

    void on_btnStopDriveWorker_clicked();

private:
    Ui::MainWindow *ui;

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

    SplashScreenManager* splashScreenManager;

    void SaveLayout();
    void RestoreLayout();

    void loadPlugins();
    void activatePlugin(QString pluginPath);


    void loadDriveParameters();
    void loadWheelParameters();

    void startDriveWorker();
    void stopDriveWorker();

    void startTelemetryWorker();

    void enable();



    NullTelemetryPlugin* nullTelemetryPlugin;
signals:
};


#endif // MAINWINDOW_H
