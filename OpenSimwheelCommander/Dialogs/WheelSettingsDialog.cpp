#include "WheelSettingsDialog.h"
#include "ui_WheelSettingsDialog.h"


WheelSettingsDialog::WheelSettingsDialog(QSettings *settings, WHEEL_PARAMETER *parameter, QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::WheelSettingsDialog)
{
    ui->setupUi(this);
    this->Settings = settings;
    this->WheelParameter = parameter;


    setFixedSize(this->size());

    connect(ui->effects_damper_slider, SIGNAL(valueChanged(int)), ui->effects_damper_spinBox, SLOT(setValue(int)));
    connect(ui->effects_damper_spinBox, SIGNAL(valueChanged(int)), ui->effects_damper_slider, SLOT(setValue(int)));

    connect(ui->effects_centerspring_spinbox, SIGNAL(valueChanged(int)), ui->effects_centerspring_slider, SLOT(setValue(int)));
    connect(ui->effects_centerspring_slider, SIGNAL(valueChanged(int)), ui->effects_centerspring_spinbox, SLOT(setValue(int)));

    connect(ui->effects_overall_slider, SIGNAL(valueChanged(int)), ui->effects_overall_spinBox, SLOT(setValue(int)));
    connect(ui->effects_overall_spinBox, SIGNAL(valueChanged(int)), ui->effects_overall_slider, SLOT(setValue(int)));

    connect(ui->wheel_center_offset_slider, SIGNAL(valueChanged(int)), ui->wheel_center_offset_spinbox, SLOT(setValue(int)));
    connect(ui->wheel_center_offset_spinbox, SIGNAL(valueChanged(int)), ui->wheel_center_offset_slider, SLOT(setValue(int)));

    connect(ui->wheel_degrees_of_rotation_slider, SIGNAL(valueChanged(int)), ui->wheel_degreesOfRotation_spinBox, SLOT(setValue(int)));
    connect(ui->wheel_degreesOfRotation_spinBox, SIGNAL(valueChanged(int)), ui->wheel_degrees_of_rotation_slider, SLOT(setValue(int)));

    InitializeValuesFromSettings();
}

WheelSettingsDialog::~WheelSettingsDialog()
{
    delete ui;
}

void WheelSettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->standardButton(button) == QDialogButtonBox::Apply) {
        SaveSettings();
    } else if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Reset) {
        InitializeValuesFromSettings();
    } else if (ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults) {
        RestoreDefaults();
    } else if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
        SaveSettings();
        this->accept();
    }

}

void WheelSettingsDialog::InitializeValuesFromSettings()
{
    ui->effects_centerspring_groupbox->setChecked(WheelParameter->CenterSpringEnabled);
        ui->effects_centerspring_spinbox->setValue(WheelParameter->CenterSpringStrength);

    ui->effects_damper_groupbox->setChecked(WheelParameter->DampingEnabled);
        ui->effects_damper_spinBox->setValue(WheelParameter->DampingStrength);

    ui->effects_overall_spinBox->setValue(WheelParameter->OverallStrength);

    ui->wheel_center_offset_groupbox->setChecked(WheelParameter->CenterOffsetEnabled);
        ui->wheel_center_offset_spinbox->setValue(WheelParameter->CenterOffset);

    ui->wheel_degreesOfRotation_spinBox->setValue(WheelParameter->DegreesOfRotation);
}

void WheelSettingsDialog::SaveSettings()
{
    Settings->setValue("WheelEffects/CenterSpringStrength", ui->effects_centerspring_spinbox->value());
    Settings->setValue("WheelEffects/DampingStrength", ui->effects_damper_spinBox->value());
    Settings->setValue("WheelEffects/OverallStrength", ui->effects_overall_spinBox->value());
    Settings->setValue("WheelEffects/DampingEnabled", ui->effects_damper_groupbox->isChecked());
    Settings->setValue("WheelEffects/CenterSpringEnabled", ui->effects_centerspring_groupbox->isChecked());

    Settings->setValue("WheelParameter/DegreesOfRotation", ui->wheel_degreesOfRotation_spinBox->value());
    Settings->setValue("WheelParameter/CenterOffset", ui->wheel_center_offset_spinbox->value());
    Settings->setValue("WheelParameter/CenterOffsetEnabled", ui->wheel_center_offset_groupbox->isChecked());

    Settings->sync();

    WheelParameter->CenterSpringStrength = ui->effects_centerspring_slider->value();
    WheelParameter->DegreesOfRotation = ui->wheel_degreesOfRotation_spinBox->value();
    WheelParameter->DampingStrength = ui->effects_damper_spinBox->value();
    WheelParameter->OverallStrength = ui->effects_overall_spinBox->value();

    WheelParameter->CenterOffsetEnabled = ui->wheel_center_offset_groupbox->isChecked();
    WheelParameter->CenterSpringEnabled = ui->effects_centerspring_groupbox->isChecked();
    WheelParameter->DampingEnabled = ui->effects_damper_groupbox->isChecked();

}

void WheelSettingsDialog::RestoreDefaults()
{
    ui->effects_centerspring_spinbox->setValue(0);
    ui->effects_damper_spinBox->setValue(50);
    ui->effects_overall_spinBox->setValue(30);
    ui->effects_damper_groupbox->setChecked(true);
    ui->effects_centerspring_groupbox->setChecked(false);

    ui->wheel_degreesOfRotation_spinBox->setValue(1080);
    ui->wheel_center_offset_spinbox->setValue(0);
    ui->wheel_center_offset_groupbox->setChecked(false);
}
