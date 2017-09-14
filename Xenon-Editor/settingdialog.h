#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    void readFromConfig();
    ~SettingDialog();

signals:
    void dataChanged();

private slots:
    void updateSettings();
    void selectInterpreter();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
