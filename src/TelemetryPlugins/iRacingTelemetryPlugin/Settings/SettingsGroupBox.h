#ifndef SETTINGSGROUPBOX_H
#define SETTINGSGROUPBOX_H

#include <QGroupBox>
#include <Settings/PluginSettings.h>

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
    void on_checkBox_useHardstops_toggled(bool checked);

    void on_smoothing_spinBox_valueChanged(int value);

    void UpdateUi();

    void on_checkBox_toggled(bool checked);

private:
    Ui::SettingsGroupBox *ui;
    QString category;
    PluginSettings* settings;

    QString carName;

    void UpdateUiComponents();
};

#endif // SETTINGSGROUPBOX_H
