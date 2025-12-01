#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

// ---------------------------------------------------------------------------------------------------
enum TokenType_t
{
    TOKEN_NUMBER,
    TOKEN_INDENT,

    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_POW,
    TOKEN_BIG_EQUAL,
    TOKEN_LIT_EQUAL,
    TOKEN_ONLY_BIG,
    TOKEN_ONLY_LIT,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

    TOKEN_EXTERN,
    TOKEN_ASSIGN,
    TOKEN_IF,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_WHILE,

    TOKEN_LEFT_ROUND,
    TOKEN_RIGHT_ROUND,
    TOKEN_DOG,
    TOKEN_COMMA,
    TOKEN_END_STR
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
struct Token_t
{
    TokenType_t type;
    char* start;
    double value;
    size_t len_name;
};
// ---------------------------------------------------------------------------------------------------


#endif /* TOKEN_TYPE_H */
