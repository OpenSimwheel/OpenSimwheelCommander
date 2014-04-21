#include "SettingsGroupBox.h"
#include "ui_settingsgroupbox.h"
#include "QTimer"

SettingsGroupBox::SettingsGroupBox(PluginSettings* settings, QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::SettingsGroupBox)
{
    ui->setupUi(this);

    connect(ui->smoothing_spinBox, SIGNAL(valueChanged(int)), ui->smoothing_slider, SLOT(setValue(int)));
    connect(ui->smoothing_slider, SIGNAL(valueChanged(int)), ui->smoothing_spinBox, SLOT(setValue(int)));
    ui->smoothing_spinBox->setToolTip(ui->smoothing_slider->toolTip());

    this->settings = settings;



    category = "General";
    UpdateUiComponents();

    QTimer* timer = new QTimer();
    timer->setInterval(1000);
    timer->start();

    connect(timer, SIGNAL(timeout()), this, SLOT(UpdateUi()));

}

void SettingsGroupBox::UpdateUiComponents() {
    ui->checkBox_useHardstops->setChecked(this->settings->GetUseHardstops());
    ui->smoothing_slider->setValue(this->settings->GetSmoothingPct());
    ui->checkBox->setChecked(this->settings->GetEnable());
}

void SettingsGroupBox::UpdateUi() {
    if (carName != settings->CarName) {
        carName = settings->CarName;
        settings->Load(carName);
        if (settings->GetEnable()) {
            UpdateUiComponents();
        } else {
            settings->Load();
            UpdateUiComponents();
        }
    }
    this->ui->checkBox->setText("Use custom settings for this car: " + settings->CarName);
}

SettingsGroupBox::~SettingsGroupBox()
{
    delete ui;
}

void SettingsGroupBox::on_checkBox_useHardstops_toggled(bool checked)
{
    this->settings->SetUseHardstops(checked);

    this->settings->Save(category);
}


void SettingsGroupBox::on_smoothing_spinBox_valueChanged(int value)
{
    this->settings->SetSmoothingPct(value);


    this->settings->Save(category);

}

void SettingsGroupBox::on_checkBox_toggled(bool checked)
{
    if (checked) {
        category = settings->CarName;
        settings->Load(category);
        UpdateUiComponents();
        settings->Save(category);
    } else {
        category = "General";
        settings->Load();
        UpdateUiComponents();
    }
}
