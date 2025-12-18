#ifndef TOKEN_FUNC_H
#define TOKEN_FUNC_H

#include "TokenType.h"
#include "VectorType.h"
#include "ErrorType.h"


// ---------------------------------------------------------------------------------------------------
int skipVoid (TokenContextInf_t* inf);
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
int     tokenGlobal (char* buffer,
                     StringTable_t* table_str,
                     TokenVector_t* vector);

Status_t tokenComment (TokenContextInf_t* inf);

Status_t tokenNumber (TokenContextInf_t* inf,
                      TokenVector_t* vector);

Status_t tokenIdent (TokenContextInf_t* inf,
                     TokenVector_t* vector);

Status_t tokenPunct (TokenContextInf_t* inf,
                     TokenVector_t* vector);

Status_t tokenOper (TokenContextInf_t* inf,
                    TokenVector_t* vector);

Status_t tokenKeyWord (TokenContextInf_t* inf,
                       TokenVector_t* vector);

int      tokenDump (TokenVector_t* vector);

int      tokenOneDump (Node_t* node);

int      tokenOneDump (Node_t* node, const char* reason);
// ---------------------------------------------------------------------------------------------------


#endif /* TOKEN_FUNC_H */
