#ifndef COMPILER_TYPE_H
#define COMPILER_TYPE_H

#include "NameTableType.h"
#include "Stack.h"


// --------------------------------------------------------------------------------------------------
struct CompilerContextInf_t
{
    NameTableStack_t* stack;
    NameTable_t* table_func;
    NameTable_t* table_type;
    FILE* stream;
    int amount_word;
    int free_point;
    Stack num_while;
    // int free_memory;
};
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
enum CompilerStatus_t
{
    CMP_THIS_OK,
    CMP_NOT_THIS,
    CMP_ERROR
};
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
enum CompilerError_t
{
    CE_JUST_ERROR,
    CE_NOT_KEY,
    CE_ASSIGN_ER,
    CE_VARDEF_ER,
    CE_COND_ER,
    CE_ELSE_OUT_IF,
    CE_RETURN,
    CE_WHILE,
    CE_REDECLAR_VAR,
    CE_EXPRESION,
    CE_BLOCK,
    CE_CHILDREN,
    CE_UNKNOWN_VAR,
    CE_UNKNOWN_FUNC,
    CE_AMOUNT_ARGS,
    CE_BREAK_OUT_WHILE
};
// --------------------------------------------------------------------------------------------------

#endif /* COMPILER_TYPE_H */
