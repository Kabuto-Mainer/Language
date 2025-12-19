#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "TypeTable.h"
#include "StringTable.h"
#include "SymbolTable.h"
#include "SupportFunc.h"


// ---------------------------------------------------------------------------------------------------
TypeTable_t* TT_Create()
{
    TypeTable_t* table = (TypeTable_t*) calloc (1, sizeof (TypeTable_t));
    if (table == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    table->types = (TypeEntry_t**) calloc (TT_START_SIZE, sizeof (TypeEntry_t*));
    if (table->types == NULL)
    {
        free (table);
        EXIT_FUNC ("NULL calloc", NULL);
    }

    table->capacity = TT_START_SIZE;
    table->count = 0;

    return table;
}

// ---------------------------------------------------------------------------------------------------
TypeEntry_t* TT_Find (TypeTable_t* table, StringEntry_t* name)
{
    assert(table);

    if (name == NULL)
        return NULL;

    for (int i = 0; i < table->count; i++)
    {
        TypeEntry_t* cur = table->types[i];
        if (cur->name == name)
            return cur;
    }
    return NULL;
}

// ---------------------------------------------------------------------------------------------------
TypeEntry_t* TT_Add (TypeTable_t* table, StringEntry_t* name,
                     TypeKind_t kind,    int size_units)
{
    assert(table);

    if (name == NULL)
        return NULL;

    TypeEntry_t* existing = TT_Find (table, name);
    if (existing != NULL)
        return existing;

    if (table->count >= table->capacity * 0.75)
        TT_Resize (table);

    TypeEntry_t* new_type = (TypeEntry_t*) calloc (1, sizeof (TypeEntry_t));
    if (new_type == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    new_type->name = name;
    new_type->kind = kind;
    new_type->size_units = size_units;

    if (kind == TYPE_STRUCT)
        new_type->info.fields_struct = SymT_Create (NULL);

    table->types[table->count++] = new_type;
    return new_type;
}

// ---------------------------------------------------------------------------------------------------
SymbolEntry_t* TT_AddStructField (TypeEntry_t* struct_type, StringEntry_t* field_name,
                                  TypeEntry_t* field_type)
{
    assert(struct_type);
    assert(struct_type->kind == TYPE_STRUCT);
    assert(field_name);
    assert(field_type);

    SymbolEntry_t* field = SymT_AddEntry (
        struct_type->info.fields_struct,
        SK_VAR,
        field_name,
        field_type
    );

    if (field == NULL)
        return NULL;

    struct_type->size_units += field_type->size_units;
    return field;
}

// ---------------------------------------------------------------------------------------------------
SymbolEntry_t* TT_FindStructField (TypeEntry_t* struct_type, StringEntry_t* field_name)
{
    assert(struct_type);
    assert(struct_type->kind == TYPE_STRUCT);
    assert(field_name);

    return SymT_FindLocal (struct_type->info.fields_struct, field_name);
}

// ---------------------------------------------------------------------------------------------------
TypeEntry_t* TT_GetArrayType (TypeTable_t* table,
                              TypeEntry_t* elem_type, int amount)
{
    assert (table);
    assert (elem_type);
    assert (amount > 0);

    for (int i = 0; i < table->count; i++)
    {
        TypeEntry_t* cur = table->types[i];

        if (cur->kind == TYPE_ARRAY &&
            cur->info.array_info.elem_type == elem_type &&
            cur->info.array_info.amount == amount)
            return cur;
    }

    if (table->count >= table->capacity * 0.75)
        TT_Resize(table);

    TypeEntry_t* new_array = (TypeEntry_t*) calloc (1, sizeof (TypeEntry_t));
    if (new_array == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    new_array->name = NULL;
    new_array->kind = TYPE_ARRAY;

    new_array->size_units = elem_type->size_units * amount;
    new_array->info.array_info.elem_type = elem_type;
    new_array->info.array_info.amount = amount;

    table->types[table->count++] = new_array;
    return new_array;
}

// ---------------------------------------------------------------------------------------------------
TypeEntry_t* TT_GetPointerType (TypeTable_t* table, TypeEntry_t* ptr_type)
{
    assert(table);
    assert(ptr_type);

    for (int i = 0; i < table->count; i++)
    {
        TypeEntry_t* cur = table->types[i];

        if (cur->kind == TYPE_POINTER &&
            cur->info.ptr_type == ptr_type)
            return cur;
    }

    if (table->count >= table->capacity * 0.75)
        TT_Resize(table);

    TypeEntry_t* new_ptr = (TypeEntry_t*) calloc (1, sizeof (TypeEntry_t));
    if (new_ptr == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    new_ptr->kind = TYPE_POINTER;
    new_ptr->name = NULL;
    new_ptr->size_units = 1;

    new_ptr->info.ptr_type = ptr_type;

    table->types[table->count++] = new_ptr;
    return new_ptr;
}

// ---------------------------------------------------------------------------------------------------
int TT_AreTypesCompatible (TypeEntry_t* type_a, TypeEntry_t* type_b)
{
    if (type_a == NULL || type_b == NULL)
        return 0;

    return (type_a == type_b) ? 1 : 0;
}

// ---------------------------------------------------------------------------------------------------
void TT_PrintType (TypeEntry_t* type, int depth)
{
    if (type == NULL)
    {
        printf("NULL");
        return;
    }

    for (int i = 0; i < depth; i++)
        printf("  ");

    switch (type->kind)
    {
        case TYPE_STANDARD:
            printf ("STANDARD: %s (size: %d units)\n",
                    type->name ? type->name->string : "unknown",
                    type->size_units);
            break;

        case TYPE_POINTER:
            printf ("POINTER (size: %d units) -> \n", type->size_units);
            TT_PrintType (type->info.ptr_type, depth + 1);
            break;

        case TYPE_ARRAY:
            printf ("ARRAY [%d] (size: %d units) of -> \n",
                    type->info.array_info.amount,
                    type->size_units);
            TT_PrintType (type->info.array_info.elem_type, depth + 1);
            break;

        case TYPE_STRUCT:
            printf ("STRUCT: %s (size: %d units)\n",
                    type->name ? type->name->string : "anonymous",
                    type->size_units);
            break;

        default:
            printf ("UNKNOWN TYPE\n");
    }
}

// ---------------------------------------------------------------------------------------------------
void TT_InitStandardTypes (TypeTable_t* t_table, StringTable_t* s_table)
{
    assert(t_table);
    assert(s_table);

    StringEntry_t* s_int = StrT_AddString (s_table, "int");
    TT_Add (t_table, s_int, TYPE_STANDARD, 1);

    StringEntry_t* s_char = StrT_AddString (s_table, "char");
    TT_Add (t_table, s_char, TYPE_STANDARD, 1);

    return;
}

// ---------------------------------------------------------------------------------------------------
void TT_Delete (TypeTable_t* table)
{
    if (table == NULL) return;

    for (int i = 0; i < table->count; i++)
    {
        TypeEntry_t* type = table->types[i];

        if (type->kind == TYPE_STRUCT && type->info.fields_struct != NULL)
            SymT_Delete (type->info.fields_struct);

        free(type);
    }

    free(table->types);
    free(table);

    return;
}

// ---------------------------------------------------------------------------------------------------
void TT_Resize (TypeTable_t* table)
{
    assert (table);

    int new_capacity = table->capacity * 2;
    TypeEntry_t** new_types = (TypeEntry_t**) realloc (table->types,
                                                        (size_t) new_capacity * sizeof (TypeEntry_t*));

    if (new_types == NULL)
        EXIT_FUNC ("NULL realloc", );

    table->types = new_types;
    table->capacity = new_capacity;

    return;
}

// ---------------------------------------------------------------------------------------------------
