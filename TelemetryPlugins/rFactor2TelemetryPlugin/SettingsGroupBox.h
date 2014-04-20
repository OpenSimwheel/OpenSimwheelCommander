#ifndef SETTINGSGROUPBOX_H
#define SETTINGSGROUPBOX_H

#include <QGroupBox>
#include <PluginSettings.h>

namespace Ui {
class SettingsGroupBox;
}

class SettingsGroupBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit SettingsGroupBox(PluginSettings* settings, QWidget *parent = 0);
    ~SettingsGroupBox();

private slots:

    void on_smoothing_spinBox_valueChanged(int value);

    void UpdateUi();

    void on_checkBox_toggled(bool checked);

    void on_max_steeringArmForce_valueChanged(int arg1);

    void on_checkBox_invert_toggled(bool checked);

private:
    Ui::SettingsGroupBox *ui;
    QString category;
    PluginSettings* settings;

    QString carName;

    void UpdateUiComponents();
};

#endif // SETTINGSGROUPBOX_H
