#ifndef CONFIGS_H
#define CONFIGS_H

#include <QFont>
#include <QSize>
#include <QPoint>
#include <QString>
#include <QSettings>
#include <QDebug>
#include <QSci/qscilexer.h>

struct Config {
public:
    bool        autoIndent;
    bool        showLineNumber;
    int         tabWidth;

    int         fontSize;
    QFont       font;

    QSize       windowSize;
    QPoint      windowPos;

    QString     interpreter;

public:
    static Config *const GetConfig();

private:
    Config() { readSettings(); }
    ~Config() { writeSettings(); }

    void readSettings();
    void writeSettings();
};

inline Config *const Config::GetConfig() {
    static Config c;
    return &c;
}

inline void Config::readSettings() {
    QSettings settings("Xenon", "Xenon Editor");
    windowPos = settings.value("windowPos", QPoint(200, 200)).toPoint();
    windowSize = settings.value("windowSize", QSize(400, 400)).toSize();
    autoIndent = settings.value("autoIndent", true).toBool();
    tabWidth = settings.value("tabWidth", 4).toInt();
    fontSize = settings.value("fontSize", 15).toInt();
    showLineNumber = settings.value("showLineNumber", true).toBool();
    interpreter = settings.value("interpreter", "Xenon-Interpreter.exe").toString();
}

inline void Config::writeSettings() {
    QSettings settings("Xenon", "Xenon Editor");
    settings.setValue("windowPos", windowPos);
    settings.setValue("windowSize", windowSize);
    settings.setValue("autoIndent", autoIndent);
    settings.setValue("tabWidth", tabWidth);
    settings.setValue("fontSize", fontSize);
    settings.setValue("showLineNumber", showLineNumber);
    settings.setValue("interpreter", interpreter);
}

#endif // CONFIGS_H
