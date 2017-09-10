#ifndef LEXERMINC_H
#define LEXERMINC_H

#include <Qsci/qscilexercpp.h>

//! \brief The LexerMinC class encapsulates the Scintilla Min-C
//! lexer.
class LexerMinC : public QsciLexerCPP {
    Q_OBJECT

public:
    const char *keywords(int set) const;
};

#endif // LEXERMINC_H
