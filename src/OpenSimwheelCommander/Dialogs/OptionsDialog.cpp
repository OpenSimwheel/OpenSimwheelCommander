#include "OptionsDialog.h"
#include "ui_OptionsDialog.h"

OptionsDialog::OptionsDialog(QSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    this->settings = settings;

    this->ui->comboBox->addItems(
        QStringList()   << "100 Hz" << "150 Hz" << "220 Hz" << "330 Hz" << "470 Hz" << "680 Hz"
                        << "1000 Hz" << "1500 Hz" << "2200 Hz" << "3300 Hz"
     );

    this->ui->comboBox->setCurrentIndex(settings->value("Program/StartupFrequency").toInt());
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::on_buttonBox_accepted()
{
    settings->setValue("Program/StartupFrequency", ui->comboBox->currentIndex());
    settings->sync();
}
