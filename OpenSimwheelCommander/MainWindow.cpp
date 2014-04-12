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
#include "Dialogs/OptionsDialog.h"

#include "TelemetryPlugins/TelemetryPluginInterface.h"
#include "TelemetryPlugins/NullTelemetryPlugin.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::hide();

    ApplicationSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());


    telemetry_feedback = TelemetryFeedback();

    WheelParameter = WHEEL_PARAMETER();
    WheelParameter.CenterSpringStrength = ApplicationSettings->value("WheelEffects/CenterSpringStrength", 0).toInt();
    WheelParameter.DegreesOfRotation = ApplicationSettings->value("WheelParameter/DegreesOfRotation", 180).toInt();
    WheelParameter.OverallStrength = ApplicationSettings->value("WheelEffects/OverallStrength", 30).toInt();
    WheelParameter.DampingStrength = ApplicationSettings->value("WheelEffects/DampingStrength", 20).toInt();
    WheelParameter.CenterOffset = ApplicationSettings->value("WheelParameter/CenterOffset", 0).toInt();
    WheelParameter.DampingEnabled = ApplicationSettings->value("WheelEffects/DampingEnabled", true).toBool();
    WheelParameter.CenterSpringEnabled = ApplicationSettings->value("WheelEffects/CenterSpringEnabled", false).toBool();
    WheelParameter.CenterOffsetEnabled = ApplicationSettings->value("WheelParameter/CenterOffsetEnabled", false).toBool();


    driveParameter = OSWDriveParameter();
    driveParameter.ComPort = ApplicationSettings->value("DriveStage/ComPort", "COM8").toString().toStdString().c_str();
    driveParameter.DeviceAddress = ApplicationSettings->value("DriveStage/DeviceAddress", 1).toInt();
    driveParameter.CommunicationTimeoutMs = ApplicationSettings->value("DriveStage/CommunicationTimeoutMs", 16).toInt();

    options = OSWOptions();
    options.StartupFrequency = ApplicationSettings->value("Settings/HomingFrequency", 1).toInt();

    driveWorker = new DriveWorker(&driveParameter, &WheelParameter, &telemetry_feedback, &options);
    driveThread = new QThread;
    driveWorker->moveToThread(driveThread);
    connect(driveThread, SIGNAL(started()), driveWorker, SLOT(process()));
    connect(driveWorker, SIGNAL(feedback_received(FEEDBACK_DATA)), this, SLOT(updateFeedbackInfo(FEEDBACK_DATA)));
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

    ui->lbl_wheel_center_offset->setText(this->WheelParameter.CenterOffsetEnabled ? QString::number(WheelParameter.CenterOffset) : "Disabled");

    ui->vjoy_dockWidget->hide();
    ui->debugInformation_dockWidget->hide();

    nullTelemetryPlugin = new NullTelemetryPlugin();

    ui->settingsWidget->setLayout(new QGridLayout);
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
        ui->lbl_debug1->setText(feedback.debug1);
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
    static quint64 count = 0;
    static quint32 countAboveTarget = 0;
    static quint32 timeouts = 0;

    if (count == 0) {
        min = data.calculationBenchmark;
        max = min;
        sum = data.calculationBenchmark;
    } else {
        if (data.calculationBenchmark < min)
            min = data.calculationBenchmark;
        else if (data.calculationBenchmark > max && data.calculationBenchmark < driveParameter.CommunicationTimeoutMs)
            max = data.calculationBenchmark;

        sum += data.calculationBenchmark;

        avg = sum / count;

        if (data.lastLoopBenchmark > MAX_LATENCY)
            if (data.lastLoopBenchmark < driveParameter.CommunicationTimeoutMs)
                countAboveTarget = countAboveTarget + 1;
            else
                timeouts = timeouts + 1;
    }



    ui->lbl_position->setText(QString::number(data.position));
    ui->lbl_torque->setText(QString::number(data.torque));
    ui->lbl_calculation_benchmark->setText(QString::number(data.calculationBenchmark / 1000.0d, 'f', 2) + " ms");
    ui->lbl_lastLoopTime->setText(QString::number(data.lastLoopBenchmark / 1000.0d, 'f', 2) + " ms");
    ui->lbl_calc_avg->setText(QString::number(avg / 1000.0d, 'f', 2) + " ms / " + QString::number(count));
    ui->lbl_calc_min->setText(QString::number(min / 1000.0d, 'f', 2) + " ms");
    ui->lbl_calc_max->setText(QString::number(max / 1000.0d, 'f', 2) + " ms");
    ui->lbl_above_target->setText(QString::number(countAboveTarget));
    ui->lbl_timeouts->setText(QString::number(timeouts));

    ui->statusBar->showMessage("Loop Frequency: " + QString::number(1000 / (data.lastLoopBenchmark / 1000.0d), 'f', 2) + "Hz");

    count = count + 1;
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
    DriveStageConfigDialog* dialog = new DriveStageConfigDialog(ApplicationSettings, &driveParameter, this);
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
    splash->showMessage("Initializing wheel...", Qt::AlignBottom | Qt::AlignRight);

    splash->showMessage(QString("Trying to connect to servo drive on SerialPort %1 & DeviceAddress %2...").arg(driveParameter.ComPort,QString::number(driveParameter.DeviceAddress)), Qt::AlignBottom | Qt::AlignRight);
    bool success = driveWorker->SmCommunicator->Connect(driveParameter.ComPort, driveParameter.DeviceAddress, driveParameter.CommunicationTimeoutMs);

    if (success == false) {
        QMessageBox::warning(this,
                             "Connetion to drive stage failed",
                             QString("Could not connect to servo drive on SerialPort %1 & DeviceAddress %2. Please check your settings!")
                             .arg(driveParameter.ComPort, QString::number(driveParameter.DeviceAddress)));
        return;
    }

    splash->showMessage("Waiting for servo drive to be ready...", Qt::AlignBottom | Qt::AlignRight);
    driveWorker->SmCommunicator->WaitForDriveReady(); //waiting for drive to finish initialization
    driveWorker->SmCommunicator->ClearFaults();
    splash->showMessage("Enabling servo drive...", Qt::AlignBottom | Qt::AlignRight);
    driveWorker->SmCommunicator->EnableDrive();

    splash->showMessage("Calibrating wheel and moving to center...", Qt::AlignBottom | Qt::AlignRight);
    driveWorker->SmCommunicator->AutoCalibrate(options.StartupFrequency, WheelParameter.CenterOffsetEnabled ?  WheelParameter.CenterOffset : 0);

    splash->showMessage("Applying settings...", Qt::AlignBottom | Qt::AlignRight);
    driveWorker->SmCommunicator->SetParameter(SMP_OSW_VELOCITY_SAMPLES, 10);
    driveWorker->SmCommunicator->SetTorqueBandwithLimit(5);
    driveWorker->SmCommunicator->SetPwmDutyCycle(1700);
    driveWorker->UpdateWheelParameter();

    splash->showMessage("Starting drive worker...", Qt::AlignBottom | Qt::AlignRight);
    driveWorker->Start();

    splash->showMessage("Ready!", Qt::AlignBottom | Qt::AlignRight);
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

    ui->comboBox_plugins->addItem("--- Please choose ---");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {

        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();

        if (plugin) {
            pluginFileNames += fileName;
            TelemetryPluginInterface *iTelemetryPlugin = qobject_cast<TelemetryPluginInterface *>(plugin);

            if (iTelemetryPlugin) {
                QJsonObject metaData = loader.metaData().value("MetaData").toObject();

                QString name = metaData.value("name").toString();
                QString version = metaData.value("version").toString();

                ui->comboBox_plugins->addItem(name + " - v" + version, pluginsDir.absoluteFilePath(fileName));
                loader.unload();
            }

        }
    }


    QString lastActivePluginPath = ApplicationSettings->value("Program/LastActivePlugin", "").toString();

    if (lastActivePluginPath != "") {
        int idx = ui->comboBox_plugins->findData(lastActivePluginPath);
        ui->comboBox_plugins->setCurrentIndex(idx);
    }
}

void MainWindow::ActivatePlugin(QString pluginPath) {
    telemetryWorker->DeleteCurrentPlugin();


    QPluginLoader loader(pluginPath);
    QObject *plugin = loader.instance();

    if (plugin) {
        TelemetryPluginInterface *iTelemetryPlugin = qobject_cast<TelemetryPluginInterface *>(plugin);

        if (iTelemetryPlugin) {
            telemetryWorker->SetPlugin(iTelemetryPlugin);

//            QMessageBox::information(this, "Plugin successfully loaded", "Successfully loaded plugin: " + pluginPath);
            QWidget* settingsWidget = iTelemetryPlugin->GetSettingsWidget();

            ui->settingsWidget->layout()->addWidget(settingsWidget);
            settingsWidget->show();

            ApplicationSettings->setValue("Program/LastActivePlugin", pluginPath);
        }
    } else {
        QMessageBox::warning(this, "Couldn't load plugin", "Couldn't load Plugin from path: " + pluginPath);
    }
}

void MainWindow::on_action_Plugins_triggered()
{
    PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
    dialog.exec();
}

void MainWindow::on_action_Options_triggered()
{
    OptionsDialog* dialog = new OptionsDialog(ApplicationSettings, this);
    dialog->setModal(true);
    dialog->show();
}

void MainWindow::on_comboBox_plugins_currentIndexChanged(int index)
{
    // clear widgets
    QList<QWidget *> widgets = ui->settingsWidget->findChildren<QWidget *>();
    foreach (QWidget *widget, widgets) {
        widget->close();
    }
//    telemetryWorker->DeleteCurrentPlugin();

    if (index <= 0) {
//        telemetryWorker->SetPlugin(nullTelemetryPlugin);

         ApplicationSettings->setValue("Program/LastActivePlugin", "");
        return;
    }


    QString pluginPath = ui->comboBox_plugins->itemData(index).toString();

    ActivatePlugin(pluginPath);
}
