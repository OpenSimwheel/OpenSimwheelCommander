#ifndef WHEELCONFIGDIALOG_H
#define WHEELCONFIGDIALOG_H

#include <QDialog>
#include <CommonStructs.h>

namespace Ui {
class WheelConfigDialog;
}

class WheelConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WheelConfigDialog(WHEEL_PARAMETER *wheelParameter, QWidget *parent = 0);
    ~WheelConfigDialog();

private:
    Ui::WheelConfigDialog *ui;
};

#endif // WHEELCONFIGDIALOG_H
