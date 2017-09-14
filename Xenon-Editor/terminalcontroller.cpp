#include "terminalcontroller.h"
#include "ui_terminalcontroller.h"
#include "configs.h"
#include <QProcess>
#include <QTextCodec>
#include <QDebug>
#include <QTextCursor>

TerminalController::TerminalController(QWidget *parent) :
    QDialog(parent),
    proc(new QProcess),
    writingManually(true),
    ui(new Ui::TerminalController) {
    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowMinMaxButtonsHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    proc = new QProcess(this);
    connect(proc, &QProcess::readyRead,
            this, &TerminalController::readFromTerminal);

    connect(ui->textEdit, &QTextEdit::textChanged,
            this, &TerminalController::write);

    readSettings();
    setWindowTitle("Terminal");
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

void TerminalController::readSettings() {
    auto c = Config::GetConfig();
    ui->textEdit->setFontPointSize(c->fontSize);
    resize(800, 600);
}

void TerminalController::closeEvent(QCloseEvent *) {
    if (proc->state() == QProcess::Running || proc->state() == QProcess::Starting) {
        proc->kill();
    }
}

void TerminalController::readFromTerminal() {
      auto line = proc->readAll();
      auto output = QTextCodec::codecForName("gb18030")->toUnicode(line);
      writingManually = false;
      ui->textEdit->insertPlainText(output);
      writingManually = true;

      auto cursor = ui->textEdit->textCursor();
      cursor.movePosition(QTextCursor::End);
      ui->textEdit->setTextCursor(cursor);
}

void TerminalController::write2Terminal() {
    if (proc->state() == QProcess::Running) {
        proc->write(cmd.toLocal8Bit());
        cmd.clear();
    }
}

void TerminalController::write() {
    if (writingManually) {
        auto text = ui->textEdit->toPlainText();
        if (text.length() > lastTextLength) {
            cmd += text[text.length() - 1];
        } else if (text.length() == lastTextLength) {
            cmd[cmd.length() - 1] = text[text.length() - 1];
        } else {
            cmd.chop(1);
        }
        lastTextLength = text.length();
        if (!cmd.isEmpty() && cmd[cmd.length() - 1] == '\n') {
            write2Terminal();
        }
    }
}
