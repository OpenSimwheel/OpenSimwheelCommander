#include "DriveStageConfigDialog.h"
#include "ui_DriveStageConfigDialog.h"
#include "QtSerialPort/qserialport.h"
#include "QtSerialPort/qserialportinfo.h"

DriveStageConfigDialog::DriveStageConfigDialog(QSettings* settings, OSWDriveParameter *driveParameter, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::DriveStageConfigDialog)
{
    ui->setupUi(this);

    setFixedSize(this->size());

    this->settings = settings;

    QStringList items;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        items << serialPortInfo.portName();

    items.sort(Qt::CaseInsensitive);

    ui->comPortComboBox->addItems(items);

    this->driveParameter = driveParameter;
    ui->comPortComboBox->setCurrentText(driveParameter->ComPort);
    ui->deviceAddressSpinBox->setValue(driveParameter->DeviceAddress);
    ui->connectionTimeoutMsSpinBox->setValue(driveParameter->CommunicationTimeoutMs);
    ui->useFastCommunicationCheckBox->setChecked(driveParameter->UseFastCommunication);
}

DriveStageConfigDialog::~DriveStageConfigDialog()
{
    delete ui;
}

void DriveStageConfigDialog::on_buttonBox_accepted()
{
    driveParameter->CommunicationTimeoutMs = ui->connectionTimeoutMsSpinBox->value();
    driveParameter->ComPort = ui->comPortComboBox->currentText().toStdString().c_str();
    driveParameter->DeviceAddress = ui->deviceAddressSpinBox->value();
    driveParameter->UseFastCommunication = ui->useFastCommunicationCheckBox->isChecked();

    this->settings->setValue("DriveStage/ComPort", driveParameter->ComPort);
    this->settings->setValue("DriveStage/DeviceAddress", driveParameter->DeviceAddress);
    this->settings->setValue("DriveStage/CommunicationTimeoutMs", driveParameter->CommunicationTimeoutMs);
    this->settings->setValue("DriveStage/UseFastCommunication", driveParameter->UseFastCommunication);
    this->settings->sync();
}
