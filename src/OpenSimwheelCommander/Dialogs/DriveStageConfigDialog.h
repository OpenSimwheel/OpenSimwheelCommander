#ifndef DRIVESTAGECONFIGDIALOG_H
#define DRIVESTAGECONFIGDIALOG_H

#include <QDialog>
#include <Qsettings>
#include "../CommonStructs.h"

namespace Ui {
class DriveStageConfigDialog;
}

class DriveStageConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DriveStageConfigDialog(QSettings* settings, OSWDriveParameter* driveParameter, QWidget *parent = 0);
    ~DriveStageConfigDialog();

    QSettings* settings;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DriveStageConfigDialog *ui;

    OSWDriveParameter* driveParameter;
};

#endif // DRIVESTAGECONFIGDIALOG_H
