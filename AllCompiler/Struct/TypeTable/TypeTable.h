#ifndef TYPE_TABLE_H
#define TYPE_TABLE_H


// ---------------------------------------------------------------------------------------------------
struct SymbolTable_t;
struct SymbolEntry_t;
// ---------------------------------------------------------------------------------------------------

/* Таблица типов реализована как динамический массив,
    т.к. их не так много и создавать для них хеш-таблицу не смысла*/
// ---------------------------------------------------------------------------------------------------
enum TypeKind_t
{
    TYPE_STANDARD,
    TYPE_STRUCT,
    TYPE_ARRAY,
    TYPE_POINTER
};

struct TypeEntry_t
{
    StringEntry_t* name;
    TypeKind_t kind;
    int size_units;

    union
    {
        TypeEntry_t* ptr_type;

        struct
        {
            struct TypeEntry_t* elem_type;
            int amount;
        } array_info;

        struct SymbolTable_t* fields_struct;
    } info;
};

struct TypeTable_t
{
    TypeEntry_t** types;
    int count;
    int capacity;
};
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
const size_t TT_START_SIZE = 10;
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
TypeTable_t*    TT_Create();

TypeEntry_t*    TT_Find                 (TypeTable_t* table,        StringEntry_t* name);

TypeEntry_t*    TT_Add                  (TypeTable_t* table,        StringEntry_t* name,
                                         TypeKind_t kind,           int size_units);

SymbolEntry_t*  TT_AddStructField       (TypeEntry_t* struct_type,  StringEntry_t* field_name,
                                         TypeEntry_t* field_type);

SymbolEntry_t*  TT_FindStructField      (TypeEntry_t* struct_type,  StringEntry_t* field_name);

TypeEntry_t*    TT_GetArrayType         (TypeTable_t* table,
                                         TypeEntry_t* elem_type,    int amount);

TypeEntry_t*    TT_GetPointerType       (TypeTable_t* table,        TypeEntry_t* ptr_type);

int             TT_AreTypesCompatible   (TypeEntry_t* type_a,       TypeEntry_t* type_b);

void            TT_PrintType            (TypeEntry_t* type,         int depth);

void            TT_InitStandardTypes    (TypeTable_t* t_table,      StringTable_t* s_table);

void            TT_Delete               (TypeTable_t* table);
// ---------------------------------------------------------------------------------------------------


#endif // TYPE_TABLE_H
