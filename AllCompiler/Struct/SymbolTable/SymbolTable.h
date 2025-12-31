#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "StringTable.h"
#include "TypeTable.h"


// ---------------------------------------------------------------------------------------------------
enum SymbolKind_t
{
    SK_VAR,
    SK_FUNC,
    SK_PARAM
};

struct SymbolEntry_t
{
    StringEntry_t* name;
    SymbolKind_t kind;

    TypeEntry_t* type;

    union {
        int offset;
        SymbolTable_t* params;
    } value;

    SymbolEntry_t* next;
};

struct SymbolTable_t
{
    SymbolEntry_t** entries;
    int count;
    int capacity;

    int current_offset;

    struct SymbolTable_t* parent;
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const int SymT_START_SIZE = 10;
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
SymbolTable_t*  SymT_Create (SymbolTable_t* parent);

SymbolEntry_t*  SymT_FindLocal (SymbolTable_t* table, StringEntry_t* name);

SymbolEntry_t*  SymT_FindGlobal (SymbolTable_t* table, StringEntry_t* name);

SymbolEntry_t*  SymT_AddEntry (SymbolTable_t* table, SymbolKind_t kind,
                               StringEntry_t* name, TypeEntry_t* type);

SymbolEntry_t*  SymT_AddFunction (SymbolTable_t* table, StringEntry_t* func_name,
                                  TypeEntry_t* return_type, SymbolTable_t* table_param_func);

SymbolEntry_t*  SymT_AddFunctionParameter (SymbolEntry_t* func_entry,
                                          StringEntry_t* param_name, TypeEntry_t* param_type);

void            SymT_Delete (SymbolTable_t* table);

void            SymT_Resize (SymbolTable_t* table);
// ---------------------------------------------------------------------------------------------------

#endif // SYMBOL_TABLE_H
