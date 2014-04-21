#include "WheelConfigDialog.h"
#include "ui_WheelConfigDialog.h"

WheelConfigDialog::WheelConfigDialog(WHEEL_PARAMETER *wheelParameter, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WheelConfigDialog)
{
    ui->setupUi(this);
}

WheelConfigDialog::~WheelConfigDialog()
{
    delete ui;
}
