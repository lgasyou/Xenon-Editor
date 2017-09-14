#ifndef TERMINALCONTROLLER_H
#define TERMINALCONTROLLER_H

#include <QDialog>
#include <QFile>
#include <QStringList>

class QProcess;
namespace Ui {
class TerminalController;
}

class TerminalController : public QDialog {
    Q_OBJECT

public:
    explicit TerminalController(QWidget *parent = 0);
    ~TerminalController();

    void init(const QString &interpreter, const QStringList &arguments);

    void start();

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void readFromTerminal();
    void write2Terminal();
    void write();

private:
    QProcess *proc;
    QString interpreter;
    QFile filein;
    QStringList arguments;
    bool writingManually;
    QString cmd;
    QString cachedText;
    Ui::TerminalController *ui;
};

#endif // TERMINALCONTROLLER_H
