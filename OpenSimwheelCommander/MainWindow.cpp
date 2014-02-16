#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "QThread"

#include "simplemotion.h"
#include "simplemotion_defs.h"
#include "simplemotion_private.h"

#include "Dialogs/DriveStageConfigDialog.h"
#include "Dialogs/WheelSettingsDialog.h"
#include "Dialogs/JoystickConfigurationDialog.h"

#include "QMessageBox"
#include "Version.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::hide();



    ApplicationSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());

    telemetry_feedback = TELEMETRY_FEEDBACK();
    count = 0;
    WheelParameter = WHEEL_PARAMETER();
    WheelParameter.CenterSpringStrength = ApplicationSettings->value("WheelEffects/CenterSpringStrength").toInt();
    WheelParameter.DegreesOfRotation = ApplicationSettings->value("WheelParameter/DegreesOfRotation").toInt();
    WheelParameter.OverallStrength = ApplicationSettings->value("WheelEffects/OverallStrength").toInt();
    WheelParameter.DampingStrength = ApplicationSettings->value("WheelEffects/DampingStrength").toInt();
    WheelParameter.CenterOffset = ApplicationSettings->value("WheelParameter/CenterOffset").toInt();
    WheelParameter.DampingEnabled = ApplicationSettings->value("WheelParameter/DampingEnabled").toBool();
    WheelParameter.CenterSpringEnabled = ApplicationSettings->value("WheelParameter/CenterSpringEnabled").toBool();
    WheelParameter.CenterOffsetEnabled = ApplicationSettings->value("WheelParameter/CenterOffsetEnabled").toBool();


    driveWorker = new DriveWorker(&WheelParameter, &telemetry_feedback);
    driveThread = new QThread;
    driveWorker->moveToThread(driveThread);
    connect(driveThread, SIGNAL(started()), driveWorker, SLOT(process()));
    connect(driveWorker, SIGNAL(feedback_received(FEEDBACK_DATA)), this, SLOT(updateFeedbackInfo(FEEDBACK_DATA)));
    connect(driveWorker, SIGNAL(homing_completed(qint32)), this, SLOT(onHomingCompleted(qint32)));
    connect(driveThread, SIGNAL(finished()), driveWorker, SLOT(deleteLater()));
    connect(driveThread, SIGNAL(finished()), driveThread, SLOT(deleteLater()));
    connect(driveWorker, SIGNAL(initializing()), this, SLOT(onWheelInitializing()));
    connect(driveWorker, SIGNAL(initialized()), this, SLOT(onWheelInitialized()));
    driveThread->start(QThread::TimeCriticalPriority);


    telemetryWorker = new TelemetryWorker;
    telemetryThread = new QThread;
    telemetryWorker->moveToThread(telemetryThread);
    connect(telemetryThread, SIGNAL(started()), telemetryWorker, SLOT(process()));
    connect(telemetryWorker, SIGNAL(telemetry_feedback_received(TELEMETRY_FEEDBACK)), this, SLOT(updateTelemetryFeedbackInfo(TELEMETRY_FEEDBACK)));
    connect(telemetryThread, SIGNAL(finished()), telemetryWorker, SLOT(deleteLater()));
    connect(telemetryThread, SIGNAL(finished()), telemetryThread, SLOT(deleteLater()));

    telemetryThread->start(QThread::HighPriority);

    ui->menu_View->addAction(ui->debugInformation_dockWidget->toggleViewAction());
    ui->menu_View->addAction(ui->telemetryOutput_dockWidget->toggleViewAction());
    ui->menu_View->addAction(ui->vjoy_dockWidget->toggleViewAction());
}

void MainWindow::updateTelemetryFeedbackInfo(TELEMETRY_FEEDBACK feedback)
{
    this->telemetry_feedback = feedback;

    ui->lbl_telemetry_torque->setText(QString::number(feedback.torque, 'f', 6) + " Nm");
    ui->lbl_telemetry_torquePct->setText(QString::number(feedback.torquePct * 100, 'f', 2) + " %");
    ui->lbl_telemetry_angle->setText(QString::number(feedback.angle, 'f', 2) + " °");
    ui->lbl_telemetry_lockToLock->setText(QString::number(feedback.steeringWheelLock, 'f', 2) + " °");
    ui->lbl_telemetry_damper->setText(QString::number(feedback.damperPct * 100, 'f', 2) + " %");
}

void MainWindow::updateFeedbackInfo(FEEDBACK_DATA data)
{
    static int countAboveTarget = 0;

    if (count == 0) {
        min = data.calculationBenchmark;
        max = min;
        sum = data.calculationBenchmark;

        if (data.lastLoopBenchmark > 4100)
            countAboveTarget++;
    } else {
        if (data.calculationBenchmark < min)
            min = data.calculationBenchmark;
        else if (data.calculationBenchmark > max)
            max = data.calculationBenchmark;

        sum += data.calculationBenchmark;

        avg = sum / count;
    }

    if (data.lastLoopBenchmark > 4100)
        countAboveTarget++;

    ui->lbl_position->setText(QString::number(data.position));
    ui->lbl_torque->setText(QString::number(data.torque));
    ui->lbl_velocity->setText(QString::number(data.velocity, 'f', 6));
    ui->lbl_calculation_benchmark->setText(QString::number(data.calculationBenchmark / 1000.0d, 'f', 2) + " ms");
    ui->lbl_lastLoopTime->setText(QString::number(data.lastLoopBenchmark / 1000.0d, 'f', 2) + " ms");
    ui->lbl_calc_avg->setText(QString::number(avg / 1000.0d, 'f', 2) + " ms / " + QString::number(count));
    ui->lbl_calc_min->setText(QString::number(min / 1000.0d, 'f', 2) + " ms");
    ui->lbl_calc_max->setText(QString::number(max / 1000.0d, 'f', 2) + " ms");

    ui->lbl_wheel_position->setText(QString::number(data.calculatedPosition));

    if (data.lastLoopBenchmark == 0)
        ui->statusBar->showMessage("Loop Frequency: >1000Hz");
    else
        ui->statusBar->showMessage("Loop Frequency: " + QString::number(1000 / (data.lastLoopBenchmark / 1000.0d), 'f', 2) + "Hz | " + QString::number(countAboveTarget) + " times above target");

    count++;
}

void MainWindow::onHomingCompleted(qint32 center)
{
    ui->lbl_wheel_center_offset->setText(QString::number(center));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_action_Quit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_action_Drive_Stage_triggered()
{
    DriveStageConfigDialog* dialog = new DriveStageConfigDialog(ApplicationSettings, this);
    dialog->setModal(true);
    dialog->show();
}

void MainWindow::on_actionWheel_Settings_triggered()
{
    WheelSettingsDialog* dialog = new WheelSettingsDialog(ApplicationSettings, &WheelParameter, this);
    dialog->setModal(true);
    dialog->show();
}

void MainWindow::on_action_About_triggered()
{
    QString version = QString("<b>OpenSimwheel Commander v%1</b>"
                              "<br /><br />"
                              "<i>Version: %1</i><br />"
                              "<i>Build Date: %2</i>"
                              "<hr>"
                              "<i>%3</i>")
                       .arg(  QString::fromLocal8Bit(VERSION_STRING),
                              QString::fromLocal8Bit(VERSION_BUILD_DATE_TIME),
                              QString::fromLocal8Bit(VERSION_COPYRIGHT));


    QMessageBox::about(this, "About OpenSimwheel Commander", version);
}

void MainWindow::on_action_Joystick_Settings_triggered()
{
    JoystickConfigurationDialog* dialog = new JoystickConfigurationDialog(this);
    dialog->setModal(true);
    dialog->show();
}

void MainWindow::onWheelInitializing()
{
    this->setEnabled(false);
    splash->showMessage("Initializing Wheel...", Qt::AlignBottom);
}

void MainWindow::onWheelInitialized()
{
    this->setEnabled(true);
//    splash->close();
    this->show();
}

void MainWindow::setSplashScreen(IndestructableSplashScreen* splash)
{
    this->splash = splash;
}

void MainWindow::on_action_Restart_triggered()
{
    QApplication::instance()->exit(1000);
}
