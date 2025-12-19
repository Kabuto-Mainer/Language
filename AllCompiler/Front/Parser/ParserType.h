#ifndef PARSER_TYPE_H
#define PARSER_TYPE_H

#include "NodeType.h"
#include "StringTable.h"
#include "TypeTable.h"

// ---------------------------------------------------------------------------------------------------
/// @brief Контекстная информация при парсинге
struct ParserContextInf_t
{
    Node_t* node;
    int line;
    int error;
    const char* name_file;
    size_t cur_index;
    size_t capacity;
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
struct ParserInf_t
{
    ParserContextInf_t* ctx;
    StringTable_t* str_table;
    TypeTable_t* type_table;
    Node_t*
}
// ---------------------------------------------------------------------------------------------------


#endif /* PARSER_TYPE_H */
