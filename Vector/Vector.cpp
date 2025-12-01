#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../Support/SupportFunc.h"
#include "VectorFunc.h"
#include "VectorConfig.h"

// ---------------------------------------------------------------------------------------------------
int vectorCtr (TokenVector_t* vector)
{
    assert (vector);

    Token_t* data = (Token_t*) calloc (VECTOR_START_CAPACITY, sizeof (Token_t));
    if (data == NULL)
        EXIT_FUNC ("NULL calloc", 1);

    vector->capacity = VECTOR_START_CAPACITY;
    vector->size = 0;
    vector->data = data;

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int vectorAdd (TokenVector_t* vector,
               const Token_t token)
{
    assert (vector);

    if (vector->size == vector->capacity)
    {
        Token_t* buffer = (Token_t*) realloc (vector->data, vector->capacity * 2 * sizeof (Token_t));
        if (buffer == NULL)
            EXIT_FUNC ("NULL realloc", 1);
        vector->data = buffer;
    }
    vector->data[vector->size++] = token;
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
size_t vectorGetSize (TokenVector_t* vector)
{
    assert (vector);
    return vector->size;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
Token_t* vectorGetToken (TokenVector_t* vector,
                         size_t index)
{
    assert (vector);

    if (index >= vector->size)
        return NULL;

    return vector->data + index;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int vectorDtr (TokenVector_t* vector)
{
    assert (vector);
    free (vector->data);
    return 0;
}
// ---------------------------------------------------------------------------------------------------
