#ifndef WHEELSETTINGSDIALOG_H
#define WHEELSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>
#include "../CommonStructs.h"

namespace Ui {
class WheelSettingsDialog;
}

class WheelSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WheelSettingsDialog(QSettings *settings, WHEEL_PARAMETER* parameter, QWidget *parent = 0);
    ~WheelSettingsDialog();

    QSettings* Settings;
    WHEEL_PARAMETER* WheelParameter;
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::WheelSettingsDialog *ui;
    void InitializeValuesFromSettings();
    void SaveSettings();
    void RestoreDefaults();
};

#endif // WHEELSETTINGSDIALOG_H
