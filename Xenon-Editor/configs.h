#ifndef CONFIGS_H
#define CONFIGS_H

#include <QFont>
#include <QSize>
#include <QPoint>
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

private:
    friend Config *const GetConfig();
    Config() {}
};

inline static Config *const GetConfig() {
    static Config c;
    return &c;
}

#endif // CONFIGS_H
