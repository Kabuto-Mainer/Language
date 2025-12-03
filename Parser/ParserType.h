#ifndef PARSER_TYPE_H
#define PARSER_TYPE_H

#include "NodeType.h"


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


#endif /* PARSER_TYPE_H */
