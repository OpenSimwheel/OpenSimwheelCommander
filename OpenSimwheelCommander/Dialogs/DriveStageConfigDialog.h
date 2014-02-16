#ifndef DRIVESTAGECONFIGDIALOG_H
#define DRIVESTAGECONFIGDIALOG_H

#include <QDialog>
#include <Qsettings>

namespace Ui {
class DriveStageConfigDialog;
}

class DriveStageConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DriveStageConfigDialog(QSettings* settings, QWidget *parent = 0);
    ~DriveStageConfigDialog();

    QSettings* settings;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DriveStageConfigDialog *ui;
};

#endif // DRIVESTAGECONFIGDIALOG_H
