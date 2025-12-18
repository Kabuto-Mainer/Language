#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "AllLexeme.h"
#include "Lexeme.h"
#include "SupportFunc.h"


// ---------------------------------------------------------------------------------------------------
LexemeDatabase_t* LexDB_Create ()
{
    LexemeDatabase_t* data_base = (LexemeDatabase_t*) calloc (1, sizeof (LexemeDatabase_t));

    if (data_base == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    data_base->keywords = (const KeywordEntry_t*) KEYWORDS;
    data_base->keywords_count = KEYWORDS_COUNT;

    data_base->punct = (const PunctEntry_t*) PUNCTUATION;
    data_base->punct_count = PUNCT_COUNT;

    data_base->operations = (const OperEntry_t*) OPERATIONS;
    data_base->operations_count = OPER_COUNT;

    return data_base;
}

// ---------------------------------------------------------------------------------------------------
int LexDB_FindKeyword (LexemeDatabase_t* data_base, const char* lexeme)
{
    assert (data_base);
    assert (lexeme);

    for (int i = 0; i < data_base->keywords_count; i++)
    {
        if (strcmp (data_base->keywords[i].lexeme, lexeme) == 0)
            return data_base->keywords[i].keyword_id;
    }
    return -1;
}

// ---------------------------------------------------------------------------------------------------
int LexDB_FindPunct (LexemeDatabase_t* data_base, const char* lexeme,
                     int* out_length,             Punct_t* out_type)
{
    assert (data_base);
    assert (lexeme);
    assert (out_length);
    assert (out_type);

    for (int i = 0; i < data_base->punct_count; i++)
    {
        if (strncmp (data_base->punct[i].lexeme, lexeme, (size_t) data_base->punct[i].length) == 0)
        {
            *out_length = data_base->punct[i].length;
            *out_type = (Punct_t) data_base->punct[i].punct_type;
            return 0;
        }
    }
    return -1;
}

// ---------------------------------------------------------------------------------------------------
int LexDB_FindOper (LexemeDatabase_t* data_base, const char* lexeme,
                    int* out_length,             Oper_t* out_type)
{
    assert (data_base);
    assert (lexeme);
    assert (out_length);
    assert (out_type);

    for (int i = 0; i < data_base->operations_count; i++)
    {
        if (strncmp (data_base->operations[i].lexeme, lexeme, (size_t) data_base->operations[i].length) == 0)
        {
            *out_length = data_base->operations[i].length;
            *out_type = (Oper_t) data_base->operations[i].op_type;
            return 0;
        }
    }
    return -1;
}

// ---------------------------------------------------------------------------------------------------
void LexDB_Destroy (LexemeDatabase_t* data_base)
{
    if (data_base)
    {
        free (data_base);
    }
    return;
}
