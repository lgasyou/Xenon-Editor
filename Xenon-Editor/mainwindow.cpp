#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QProcess>
#include <QDebug>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciapis.h>

#include "mainwindow.h"
#include "lexerminc.h"
#include "configs.h"
#include "terminalcontroller.h"

MainWindow::MainWindow() {
    textEdit = new QsciScintilla;
    textEdit->setUtf8(true);
    terminalController = new TerminalController();
    textLexer = new LexerMinC;
    textLexer->setColor(QColor(Qt::blue), QsciLexerCPP::KeywordSet2);
    textEdit->setLexer(textLexer);
    setupApis();
    textEdit->setAutoCompletionSource(QsciScintilla::AcsAll);
    textEdit->setAutoCompletionCaseSensitivity(true);
    textEdit->setAutoCompletionThreshold(1);
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createStatusBar();

    readSettings();

    connect(textEdit, SIGNAL(textChanged()),
            this, SLOT(documentWasModified()));

    setCurrentFile("");
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile() {
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile("");
    }
}

void MainWindow::open() {
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save() {
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs() {
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::changeSettings() {
    // 改变设置
}

void MainWindow::about() {
   QMessageBox::about(this, tr("About Xenon Editor"),
            tr("The <b>Xenon Editor</b> is an editor which is "
               "created with the purpose of editing the Min-C code."));
}

void MainWindow::documentWasModified() {
    setWindowModified(textEdit->isModified());
}

void MainWindow::run() {
    QStringList arguments;
    arguments << curFile;
    terminalController->init(GetConfig()->interpreter, arguments);
    terminalController->start();
    terminalController->show();
}

void MainWindow::createActions() {
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    settingAct = new QAction(tr("Settings"), this);
    settingAct->setStatusTip(tr("Open Settings dialog"));
    connect(settingAct, &QAction::triggered, this, &MainWindow::changeSettings);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit Xenon Editor"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    runAct = new QAction(tr("Run"), this);
    runAct->setShortcut(tr("Ctrl+R"));
    runAct->setStatusTip(tr("Run this program"));
    connect(runAct, SIGNAL(triggered()), this, SLOT(run()));

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show Xenon Editor's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(settingAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    runMenu = menuBar()->addMenu(tr("&Run"));
    runMenu->addAction(runAct);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings() {
    QSettings settings("Xenon", "Xenon Editor");
    auto c = GetConfig();
    c->windowPos = settings.value("pos", QPoint(200, 200)).toPoint();
    c->windowSize = settings.value("size", QSize(400, 400)).toSize();
    c->autoIndent = settings.value("autoIndent", true).toBool();
    c->tabWidth = settings.value("tabWidth", 4).toInt();
    c->fontSize = settings.value("fontSize", 15).toInt();
    c->showLineNumber = settings.value("showLineNumber", true).toBool();
    c->interpreter = settings.value("interpreter", "E:/Documents/GitHub/Xenon-Editor/build-Xenon-Editor-Desktop_Qt_5_9_1_MSVC2017_64bit-Release/release/Terminal.exe").toString();

    resize(c->windowSize);
    move(c->windowPos);
    textEdit->setAutoIndent(c->autoIndent);
    textEdit->setTabWidth(c->tabWidth);
    auto lexer = textEdit->lexer();
    auto font = QFont(); font.setPointSize(c->fontSize);
    lexer->setFont(font);

    if (c->showLineNumber) {
        textEdit->setMarginType(0, QsciScintilla::NumberMargin);
        textEdit->setMarginLineNumbers(0, true);
        textEdit->setMarginWidth(0, 30);
    }
}

void MainWindow::writeSettings() {
    QSettings settings("Xenon", "Xenon Editor");
    auto c = GetConfig();
    settings.setValue("windowPos", c->windowPos);
    settings.setValue("windowSize", c->windowSize);
    settings.setValue("autoIndent", c->autoIndent);
    settings.setValue("tabWidth", c->tabWidth);
    settings.setValue("fontSize", c->fontSize);
    settings.setValue("showLineNumber", c->showLineNumber);
    settings.setValue("interpreter", c->interpreter);
}

bool MainWindow::maybeSave() {
    if (textEdit->isModified()) {
        int ret = QMessageBox::warning(this, tr("Xenon Editor"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->text();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName) {
    curFile = fileName;
    textEdit->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Xenon Editor")));
}

QString MainWindow::strippedName(const QString &fullFileName) {
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::setupApis() {
    apis = new QsciAPIs(textLexer);
    apis->add("int");
    apis->add("real");
    apis->add("string");

    apis->add("if");
    apis->add("else");
    apis->add("while");
    apis->add("return");
    apis->add("in");
    apis->add("out");
    apis->add("do");
    apis->add("until");
    apis->add("for");

    apis->prepare();
}
