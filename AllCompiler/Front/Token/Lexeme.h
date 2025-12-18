#ifndef LEXEME_H
#define LEXEME_H

#include "AllLexeme.h"


// ---------------------------------------------------------------------------------------------------
LexemeDatabase_t*   LexDB_Create ();

int                 LexDB_FindKeyword   (LexemeDatabase_t* data_base,   const char* lexeme);

int                 LexDB_FindPunct     (LexemeDatabase_t* data_base,   const char* lexeme,
                                         int* out_length,               Punct_t* out_type);

int                 LexDB_FindOper      (LexemeDatabase_t* data_base,   const char* lexeme,
                                         int* out_length,               Oper_t* out_type);

void                LexDB_Destroy       (LexemeDatabase_t* data_base);
// ---------------------------------------------------------------------------------------------------

#endif /* LEXEME_H */
