#include "DriveStageConfigDialog.h"
#include "ui_DriveStageConfigDialog.h"
#include "QtSerialPort/qserialport.h"
#include "QtSerialPort/qserialportinfo.h"

DriveStageConfigDialog::DriveStageConfigDialog(QSettings* settings, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::DriveStageConfigDialog)
{
    ui->setupUi(this);

    setFixedSize(this->size());

    this->settings = settings;

    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
        ui->comPortComboBox->addItem(serialPortInfo.portName());

    ui->comPortComboBox->setCurrentText(this->settings->value("DriveStage/ComPort").toString());
    ui->deviceAddressSpinBox->setValue(this->settings->value("DriveStage/DeviceAddress").toInt());

}

DriveStageConfigDialog::~DriveStageConfigDialog()
{
    delete ui;
}

void DriveStageConfigDialog::on_buttonBox_accepted()
{
    this->settings->setValue("DriveStage/ComPort", ui->comPortComboBox->currentText());
    this->settings->setValue("DriveStage/DeviceAddress", ui->deviceAddressSpinBox->value());
    this->settings->sync();
}
