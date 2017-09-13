#include "terminalcontroller.h"
#include "ui_terminalcontroller.h"
#include <QProcess>
#include <QTextCodec>
#include <QDebug>
#include <QTextCursor>

TerminalController::TerminalController(QWidget *parent) :
    QDialog(parent),
    proc(new QProcess),
    ui(new Ui::TerminalController) {
    ui->setupUi(this);

    proc = new QProcess(this);
    connect(proc, &QProcess::readyRead,
            this, &TerminalController::readFromTerminal);

    connect(ui->pushButton_confirm, &QPushButton::clicked,
            this, &TerminalController::write2Terminal);

    ui->textEdit->setFontPointSize(15);
}

TerminalController::~TerminalController() {
    delete proc;
    delete ui;
}

void TerminalController::init(const QString &interpreter, const QStringList &arguments) {
    this->interpreter = interpreter;
    this->arguments = arguments;
}

void TerminalController::start() {
    proc->start(interpreter, arguments);
    ui->textEdit->clear();
}

void TerminalController::closeEvent(QCloseEvent *) {
    if (proc->state() == QProcess::Running || proc->state() == QProcess::Starting) {
        proc->kill();
    }
}

void TerminalController::readFromTerminal() {
      auto line = proc->readAll();
      auto output = QTextCodec::codecForName("gb18030")->toUnicode(line);
      ui->textEdit->insertPlainText(output);

      auto cursor = ui->textEdit->textCursor();
      cursor.movePosition(QTextCursor::End);
      ui->textEdit->setTextCursor(cursor);
}

void TerminalController::write2Terminal() {
    auto cmd = ui->lineEdit->text() + "\n";
    ui->textEdit->insertPlainText(cmd);
    proc->write(cmd.toLocal8Bit());
    ui->lineEdit->clear();
}
