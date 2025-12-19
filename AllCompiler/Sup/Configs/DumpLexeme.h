#ifndef DUMP_LEXEME_H
#define DUMP_LEXEME_H


const char DUMP_KEYWORDS[][20] =
{
    "define var",   "alloc memory",     "input",        "output",       "print",
    "if",           "elif"              "else",         "while",        "break",
    "continue",     "define func",      "return",       "call func",    "exit"
};

const char DUMP_OPER[][10] =
{
    "+",            "-",                "*",            "/",            "==",
    "!=",           "<",                "<=",           ">",            ">=",
    "&&",           "||",               "!",            "deref",        "adr",
    "="
};


#endif /* DUMP_LEXEME_H */
