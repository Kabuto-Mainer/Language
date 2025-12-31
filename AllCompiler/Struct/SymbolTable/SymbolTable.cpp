#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "SymbolTable.h"
#include "SupportFunc.h"


// ---------------------------------------------------------------------------------------------------
SymbolTable_t* SymT_Create (SymbolTable_t* parent)
{
    /* Проверку на NULL делать нельзя */
    SymbolTable_t* table = (SymbolTable_t*) calloc (1, sizeof (SymbolTable_t));
    if (table == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    table->entries = (SymbolEntry_t**) calloc (SymT_START_SIZE, sizeof (SymbolEntry_t*));
    if (table->entries == NULL)
    {
        free (table);
        EXIT_FUNC ("NULL calloc", NULL);
    }

    table->parent = parent;
    table->count = 0;
    table->capacity = SymT_START_SIZE;

    if (parent)
        table->current_offset = parent->current_offset;
    else
        table->current_offset = 0;

    return table;
}

SymbolEntry_t* SymT_FindGlobal (SymbolTable_t* table, StringEntry_t* name)
{
    assert (table);
    assert (name);

    SymbolEntry_t* entry = SymT_FindLocal (table, name);
    if (entry != NULL)
        return entry;

    if (table->parent != NULL)
        return SymT_FindGlobal (table->parent, name);

    return NULL;
}

SymbolEntry_t* SymT_FindLocal (SymbolTable_t* table, StringEntry_t* name)
{
    assert (table);
    assert (name);

    size_t index = name->hash % (size_t) table->capacity;
    SymbolEntry_t* current = table->entries[index];

    while (current != NULL)
    {
        if (current->name == name)
            return current;

        current = current->next;
    }
    return NULL;
}

SymbolEntry_t* SymT_AddEntry (SymbolTable_t* table, SymbolKind_t kind,
                              StringEntry_t* name, TypeEntry_t* type)
{
    assert (table);
    assert (name);
    assert (type);

    if (SymT_FindLocal (table, name) != NULL)
        return NULL;

    if (table->count * 0.75 >= table->capacity)
        SymT_Resize (table);

    SymbolEntry_t* new_entry = (SymbolEntry_t*) calloc (1, sizeof (SymbolEntry_t));
    if (new_entry == NULL)
        EXIT_FUNC("NULL calloc", NULL);

    new_entry->name = name;
    new_entry->kind = kind;
    new_entry->type = type;

    int size = type->size_units;
    new_entry->value.offset = table->current_offset;
    table->current_offset += size;

    size_t index = name->hash % (size_t) table->capacity;
    new_entry->next = table->entries[index];
    table->entries[index] = new_entry;

    table->count++;
    return new_entry;
}

SymbolEntry_t* SymT_AddFunction (SymbolTable_t* table, StringEntry_t* func_name,
                                 TypeEntry_t* return_type, SymbolTable_t* table_param_func)
{
    assert (table);
    assert (func_name);
    assert (return_type);
    assert (table_param_func);

    if (SymT_FindLocal (table, func_name) != NULL)
        return NULL;

    if (table->count * 0.75 >= table->capacity)
        SymT_Resize (table);

    SymbolEntry_t* func_entry = (SymbolEntry_t*) calloc (1, sizeof (SymbolEntry_t));
    if (func_entry == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    func_entry->name = func_name;
    func_entry->kind = SK_FUNC;
    func_entry->type = return_type;
    func_entry->value.params = table_param_func;

    size_t index = func_name->hash % (size_t) table->capacity;
    func_entry->next = table->entries[index];
    table->entries[index] = func_entry;

    table->count++;
    return func_entry;
}

SymbolEntry_t* SymT_AddFunctionParameter (SymbolEntry_t* func_entry,
                                          StringEntry_t* param_name, TypeEntry_t* param_type)
{
    assert (func_entry);
    assert (func_entry->kind == SK_FUNC);
    assert (param_name);
    assert (param_type);

    SymbolEntry_t* param = SymT_AddEntry (func_entry->value.params, SK_PARAM, param_name, param_type);
    return param;
}


void SymT_Delete (SymbolTable_t* table)
{
    if (table == NULL)
        return;

    for (int i = 0; i < table->capacity; i++)
    {
        SymbolEntry_t* current = table->entries[i];
        while (current != NULL)
        {
            SymbolEntry_t* next = current->next;
            free (current);
            current = next;
        }
    }

    free(table->entries);
    free(table);
    return;
}

void SymT_Resize (SymbolTable_t* table)
{
    assert (table);

    int new_capacity = table->capacity * 2;
    SymbolEntry_t** new_entries = (SymbolEntry_t**) calloc ((size_t) new_capacity, sizeof (SymbolEntry_t*));

    if (new_entries == NULL)
        EXIT_FUNC ("NULL calloc", );

    for (int i = 0; i < table->capacity; i++)
    {
        SymbolEntry_t* current = table->entries[i];
        while (current != NULL)
        {
            SymbolEntry_t* next = current->next;
            size_t new_index = current->name->hash % (size_t) new_capacity;

            current->next = new_entries[new_index];
            new_entries[new_index] = current;

            current = next;
        }
    }

    free (table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;

    return;
}
// ---------------------------------------------------------------------------------------------------
