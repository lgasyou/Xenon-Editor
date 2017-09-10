#include "lexerminc.h"

const char *LexerMinC::keywords(int set) const {
    if(set == 1 || set == 3)
        return QsciLexerCPP::keywords(set);
    if(set == 2)
        return "out in real string until";

    return 0;
}
