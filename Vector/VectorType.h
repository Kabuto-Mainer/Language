#ifndef VECTOR_TYPE_H
#define VECTOR_TYPE_H

#include "../Token/TokenType.h"

// ---------------------------------------------------------------------------------------------------
struct TokenVector_t
{
    Token_t* data;
    size_t size;
    size_t capacity;
};
// ---------------------------------------------------------------------------------------------------


#endif /* VECTOR_TYPE_H */
