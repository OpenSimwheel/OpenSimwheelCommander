#ifndef JOYSTICKCONFIGURATIONDIALOG_H
#define JOYSTICKCONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class JoystickConfigurationDialog;
}

class JoystickConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoystickConfigurationDialog(QWidget *parent = 0);
    ~JoystickConfigurationDialog();

private:
    Ui::JoystickConfigurationDialog *ui;
};

#endif // JOYSTICKCONFIGURATIONDIALOG_H
