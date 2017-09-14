#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "configs.h"
#include <QDebug>
#include <QFileDialog>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog) {
    ui->setupUi(this);
    readFromConfig();
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &SettingDialog::updateSettings);
    connect(ui->pushButton_interpreter, &QPushButton::clicked,
            this, &SettingDialog::selectInterpreter);

    setWindowTitle(tr("Settings"));
}

void SettingDialog::readFromConfig() {
    auto c = Config::GetConfig();
    ui->checkBox_autoIndent->setChecked(c->autoIndent);
    ui->checkBox_showLineNumber->setChecked(c->showLineNumber);
    ui->spinBox_tabWidth->setValue(c->tabWidth);
    ui->spinBox_fontSize->setValue(c->fontSize);
    ui->lineEdit_interpreter->setText(c->interpreter);
}

SettingDialog::~SettingDialog() {
    delete ui;
}

void SettingDialog::updateSettings() {
    auto c = Config::GetConfig();
    c->autoIndent = ui->checkBox_autoIndent->isChecked();
    c->showLineNumber = ui->checkBox_showLineNumber->isChecked();
    c->tabWidth = ui->spinBox_tabWidth->value();
    c->fontSize = ui->spinBox_fontSize->value();
    c->interpreter = ui->lineEdit_interpreter->text();
    emit dataChanged();
    accept();
}

void SettingDialog::selectInterpreter() {
    auto fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) {
        ui->lineEdit_interpreter->setText(fileName);
    }
}
