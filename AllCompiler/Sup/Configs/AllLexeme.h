#ifndef LEXEME_DATABASE_H
#define LEXEME_DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TypesOfType.h"


// ---------------------------------------------------------------------------------------------------
struct KeywordEntry_t
{
    const char* lexeme;
    int keyword_id;
};

struct PunctEntry_t
{
    const char* lexeme;
    int punct_type;
    int length;
};

struct OperEntry_t
{
    const char* lexeme;
    Oper_t op_type;
    int length;
};

struct LexemeDatabase_t
{
    const KeywordEntry_t* keywords;
    int keywords_count;

    const PunctEntry_t* punct;
    int punct_count;

    const OperEntry_t* operations;
    int operations_count;
};
// ---------------------------------------------------------------------------------------------------

/// \brief Все ключевые слова
enum KeywordID_t
{
    KW_VARIABLE     = 0,
    KW_ALLOCATE     = 1,
    KW_INPUT        = 2,
    KW_OUTPUT       = 3,
    KW_PRINT        = 4,
    KW_IF           = 5,
    KW_ELIF         = 6,
    KW_ELSE         = 7,
    KW_WHILE        = 8,
    KW_BREAK        = 9,
    KW_CONTINUE     = 10,
    KW_FUNCTION     = 11,
    KW_RETURN       = 12,
    KW_CALL         = 13,
    KW_EXIT         = 14,
    KW_INTEGER      = 15,
    KW_STRING       = 16,
};

// ---------------------------------------------------------------------------------------------------
const KeywordEntry_t KEYWORDS[] =
{
    {"variable",        KW_VARIABLE},
    {"allocate",        KW_ALLOCATE},
    {"input",           KW_INPUT},
    {"output",          KW_OUTPUT},
    {"print",           KW_PRINT},
    {"if",              KW_IF},
    {"elif",            KW_ELIF},
    {"else",            KW_ELSE},
    {"while",           KW_WHILE},
    {"break",           KW_BREAK},
    {"continue",        KW_CONTINUE},
    {"function",        KW_FUNCTION},
    {"return",          KW_RETURN},
    {"call",            KW_CALL},
    {"exit",            KW_EXIT},
    // {"int",             KW_INTEGER},
    // {"str",             KW_STRING}
};

const int KEYWORDS_COUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);
// ---------------------------------------------------------------------------------------------------
const PunctEntry_t PUNCTUATION[] =
{
    {"\n",      PUNCT_ENDSTRING,     1},
    {",",       PUNCT_COMMA,         1},
    {"(",       PUNCT_LPAREN,        1},
    {")",       PUNCT_RPAREN,        1},
    {"<",       PUNCT_LTANG,         1},
    {">",       PUNCT_RTANG,         1},
    {"[",       PUNCT_LBRACKET,      1},
    {"]",       PUNCT_RBRACKET,      1},
    {".",       PUNCT_DOT,           1},
    {"#",       PUNCT_COMMENT,       1},
    {":",       PUNCT_COLON,         1},
    {"->",      PUNCT_ARROW,         2},
    {"=>",      PUNCT_ARROW,         2},
};

const int PUNCT_COUNT = sizeof(PUNCTUATION) / sizeof(PUNCTUATION[0]);
// ---------------------------------------------------------------------------------------------------
const OperEntry_t OPERATIONS[] =
{
    {"+",               OPER_ADD,        1},
    {"-",               OPER_SUB,        1},
    {"*",               OPER_MUL,        1},
    {"/",               OPER_DIV,        1},

    {"equal",           OPER_E,          5},
    {"notequal",        OPER_NE,         8},
    {"less",            OPER_L,          4},
    {"less equal",      OPER_LE,         10},
    {"greater",         OPER_B,          7},
    {"greater equal",   OPER_BE,         13},

    {"&&",              OPER_AND,        2},
    {"||",              OPER_OR,         2},
    {"!",               OPER_NOT,        1},

    {"s:*",             OPER_DEREF,      3},
    {"s:&",             OPER_ADDR,       3},
    {"=",               OPER_ASSIGN,     1}
};

const int OPER_COUNT = sizeof(OPERATIONS) / sizeof(OPERATIONS[0]);
// ---------------------------------------------------------------------------------------------------


#endif /* LEXEME_DATABASE_H */
