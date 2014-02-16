#include "JoystickConfigurationDialog.h"
#include "ui_JoystickConfigurationDialog.h"

JoystickConfigurationDialog::JoystickConfigurationDialog(QWidget *parent) :
     QDialog(parent, Qt::Tool),
    ui(new Ui::JoystickConfigurationDialog)
{
    ui->setupUi(this);

    setFixedSize(this->size());
}

JoystickConfigurationDialog::~JoystickConfigurationDialog()
{
    delete ui;
}
