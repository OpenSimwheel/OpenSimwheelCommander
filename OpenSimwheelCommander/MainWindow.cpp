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

#include "QPluginLoader"

#include "Dialogs/PluginDialog.h"

#include "TelemetryPlugins/TelemetryPluginInterface.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::hide();

    count = 0;

    ApplicationSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());


    telemetry_feedback = TelemetryFeedback();

    WheelParameter = WHEEL_PARAMETER();
    WheelParameter.CenterSpringStrength = ApplicationSettings->value("WheelEffects/CenterSpringStrength").toInt();
    WheelParameter.DegreesOfRotation = ApplicationSettings->value("WheelParameter/DegreesOfRotation").toInt();
    WheelParameter.OverallStrength = ApplicationSettings->value("WheelEffects/OverallStrength").toInt();
    WheelParameter.DampingStrength = ApplicationSettings->value("WheelEffects/DampingStrength").toInt();
    WheelParameter.CenterOffset = ApplicationSettings->value("WheelParameter/CenterOffset").toInt();
    WheelParameter.DampingEnabled = ApplicationSettings->value("WheelEffects/DampingEnabled").toBool();
    WheelParameter.CenterSpringEnabled = ApplicationSettings->value("WheelEffects/CenterSpringEnabled").toBool();
    WheelParameter.CenterOffsetEnabled = ApplicationSettings->value("WheelParameter/CenterOffsetEnabled").toBool();
    WheelParameter.ComPort = ApplicationSettings->value("DriveStage/ComPort").toString().toStdString().c_str();
    WheelParameter.DeviceAddress = ApplicationSettings->value("DriveStage/DeviceAddress").toInt();

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
    connect(driveWorker->Joystick, SIGNAL(Initialized(JoystickDriverInfo,JoystickDeviceInfo)), this, SLOT(onJoystickInitialized(JoystickDriverInfo,JoystickDeviceInfo)));
    connect(driveWorker->Joystick, SIGNAL(PositionUpdated(qint32)), this, SLOT(onJoystickPositionUpdated(qint32)));
    driveThread->start(QThread::TimeCriticalPriority);


    telemetryWorker = new TelemetryWorker;
    telemetryThread = new QThread;
    telemetryWorker->moveToThread(telemetryThread);
    connect(telemetryThread, SIGNAL(started()), telemetryWorker, SLOT(process()));
    connect(telemetryWorker, SIGNAL(telemetry_feedback_received(TelemetryFeedback)), this, SLOT(updateTelemetryFeedbackInfo(TelemetryFeedback)));
    connect(telemetryThread, SIGNAL(finished()), telemetryWorker, SLOT(deleteLater()));
    connect(telemetryThread, SIGNAL(finished()), telemetryThread, SLOT(deleteLater()));

    telemetryThread->start(QThread::HighPriority);

    ui->menu_View->addAction(ui->debugInformation_dockWidget->toggleViewAction());
    ui->menu_View->addAction(ui->vjoy_dockWidget->toggleViewAction());


    ui->vjoy_dockWidget->hide();
    ui->debugInformation_dockWidget->hide();

    LoadPlugins();
}

void MainWindow::updateTelemetryFeedbackInfo(TelemetryFeedback feedback)
{
    this->telemetry_feedback = feedback;

    bool isConnected = feedback.isConnected;

    if (isConnected) {
        ui->lbl_telemetry_status->setText("connected");
        ui->groupBox_telemetryDebug->setEnabled(true);

        ui->lbl_telemetry_torque->setText(QString::number(feedback.torque, 'f', 6) + " Nm");
        ui->lbl_telemetry_torquePct->setText(QString::number(feedback.torquePct * 100, 'f', 2) + " %");
        ui->lbl_telemetry_angle->setText(QString::number(feedback.angle, 'f', 2) + " °");
        ui->lbl_telemetry_lockToLock->setText(QString::number(feedback.steeringWheelLock, 'f', 2) + " °");
        ui->lbl_telemetry_damper->setText(QString::number(feedback.damperPct * 100, 'f', 2) + " %");
    } else {
        ui->groupBox_telemetryDebug->setEnabled(false);
        ui->lbl_telemetry_status->setText("disconnected");
        ui->lbl_telemetry_torque->setText("-");
        ui->lbl_telemetry_torquePct->setText("-");
        ui->lbl_telemetry_angle->setText("-");
        ui->lbl_telemetry_lockToLock->setText("-");
        ui->lbl_telemetry_damper->setText("-");
    }

}

void MainWindow::updateFeedbackInfo(FEEDBACK_DATA data)
{
    static int countAboveTarget = 0;

    if (count == 0) {
        min = data.calculationBenchmark;
        max = min;
        sum = data.calculationBenchmark;

        if (data.lastLoopBenchmark > MAX_LATENCY)
            countAboveTarget++;
    } else {
        if (data.calculationBenchmark < min)
            min = data.calculationBenchmark;
        else if (data.calculationBenchmark > max)
            max = data.calculationBenchmark;

        sum += data.calculationBenchmark;

        avg = sum / count;
    }

    if (data.lastLoopBenchmark > MAX_LATENCY)
        countAboveTarget++;

    ui->lbl_position->setText(QString::number(data.position));
    ui->lbl_torque->setText(QString::number(data.torque));
    ui->lbl_calculation_benchmark->setText(QString::number(data.calculationBenchmark / 1000.0d, 'f', 2) + " ms");
    ui->lbl_lastLoopTime->setText(QString::number(data.lastLoopBenchmark / 1000.0d, 'f', 2) + " ms");
    ui->lbl_calc_avg->setText(QString::number(avg / 1000.0d, 'f', 2) + " ms / " + QString::number(count));
    ui->lbl_calc_min->setText(QString::number(min / 1000.0d, 'f', 2) + " ms");
    ui->lbl_calc_max->setText(QString::number(max / 1000.0d, 'f', 2) + " ms");

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
    this->close();
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
    splash->showMessage("Initializing Wheel...", Qt::AlignBottom | Qt::AlignRight);
}

void MainWindow::onWheelInitialized()
{
    this->setEnabled(true);
    RestoreLayout();
    this->splash->close();
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

void MainWindow::closeEvent(QCloseEvent *)
{
    SaveLayout();
}

void MainWindow::SaveLayout()
{
    ApplicationSettings->setValue("Window/Geometry", saveGeometry());
    ApplicationSettings->setValue("Window/State", saveState());
    ApplicationSettings->sync();
}

void MainWindow::RestoreLayout()
{
    restoreGeometry(ApplicationSettings->value("Window/Geometry").toByteArray());
    restoreState(ApplicationSettings->value("Window/State").toByteArray());
}


void MainWindow::onJoystickInitialized(JoystickDriverInfo driverInfo, JoystickDeviceInfo deviceInfo) {

    ui->lbl_vjoy_enabled->setText(driverInfo.Enabled ? "true" : "false");
    ui->lbl_vjoy_vendor->setText(driverInfo.Vendor);
    ui->lbl_vjoy_product->setText(driverInfo.Product);
    ui->lbl_vjoy_version->setText(driverInfo.VersionNumber);
    ui->lbl_vjoy_device_status->setText(deviceInfo.Status);

    ui->lbl_vjoy_axis_min->setText(QString::number(deviceInfo.AxisMin));
    ui->lbl_vjoy_axis_max->setText(QString::number(deviceInfo.AxisMax));
    ui->lbl_vjoy_device_id->setText(QString::number(deviceInfo.Id));

}

void MainWindow::onJoystickPositionUpdated(qint32 pos) {
    ui->lbl_vjoy_axis_pos->setText(QString::number(pos));
}

void MainWindow::LoadPlugins() {
    pluginsDir = QApplication::applicationDirPath();
    pluginsDir.cd("plugins");

//    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
//        pluginsDir.cdUp();

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        //ui->comboBox_plugins->addItem(fileName);

        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();

        if (plugin) {
            pluginFileNames += fileName;
            TelemetryPluginInterface *iTelemetryPlugin = qobject_cast<TelemetryPluginInterface *>(plugin);

            if (iTelemetryPlugin) {
                QJsonObject metaData = loader.metaData().value("MetaData").toObject();

                QString name = metaData.value("name").toString();
                QString version = metaData.value("version").toString();

                ui->comboBox_plugins->addItem(name + " - v" + version);
                telemetryWorker->SetPlugin(iTelemetryPlugin);
            }

        }
    }
}

void MainWindow::on_action_Plugins_triggered()
{
    PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
    dialog.exec();
}
