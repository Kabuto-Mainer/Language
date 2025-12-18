#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include "TypesOfType.h"
#include "NodeType.h"
#include "StringTable.h"
#include "Lexeme.h"

// ---------------------------------------------------------------------------------------------------
/// @brief Контекстная информация при токенизации
struct TokenContextInf_t
{
    char** pose;
    int line;
    StringTable_t* table_str;
    LexemeDatabase_t* lexeme_data;
};

struct SystemSyntaxWord_t
{
    const char* name;
    NodeType_t type;
    int value;
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const int TOKEN_MAX_LEN = 32;
// ---------------------------------------------------------------------------------------------------


#endif /* TOKEN_TYPE_H */
