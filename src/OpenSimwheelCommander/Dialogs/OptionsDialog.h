#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <Qsettings>
#include "../CommonStructs.h"

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QSettings *settings, QWidget *parent = 0);
    ~OptionsDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::OptionsDialog *ui;

    QSettings* settings;
};

#endif // OPTIONSDIALOG_H
