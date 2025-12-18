#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "StringTable.h"
#include "SupportFunc.h"


// ---------------------------------------------------------------------------------------------------
StringTable_t* StrT_Create ()
{
    StringTable_t* table = (StringTable_t*) calloc (1, sizeof (StringTable_t));
    if (table == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    table->count = 0;
    table->size = StrT_START_SIZE;

    table->buckets = (StringEntry_t**) calloc (StrT_START_SIZE, sizeof (StringEntry_t*));
    if (table->buckets == NULL)
    {
        free (table);
        EXIT_FUNC ("NULL calloc", NULL);
    }

    return table;
}

StringEntry_t* StrT_AddString (StringTable_t* table, char* name)
{
    assert(table);
    assert(name);

    size_t hash = getHash (name);
    size_t index = hash % table->size;

    StringEntry_t* current = table->buckets[index];
    while (current != NULL)
    {
        if (current->hash == hash && strcmp (current->string, name) == 0)
            return current;

        current = current->next;
    }

    if (table->count * 0.75 >= table->size)
        StrT_Resize (table);

    StringEntry_t* new_entry = (StringEntry_t*) calloc (1, sizeof (StringEntry_t));
    if (new_entry == NULL)
        EXIT_FUNC ("NULL calloc", NULL);

    new_entry->string = strdup (name);
    new_entry->hash = hash;

    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    table->count++;

    return new_entry;
}

void StrT_Resize (StringTable_t* table)
{
    size_t new_size = table->size * 2;
    StringEntry_t** new_buckets = (StringEntry_t**) calloc (new_size, sizeof (StringEntry_t*));

    if (new_buckets == NULL)
        EXIT_FUNC ("NULL calloc", );

    for (size_t i = 0; i < table->size; i++)
    {
        StringEntry_t* current = table->buckets[i];
        while (current != NULL)
        {
            StringEntry_t* next = current->next;

            size_t new_index = current->hash % new_size;
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next;
        }
    }

    free (table->buckets);
    table->buckets = new_buckets;
    table->size = new_size;

    return;
}

void StrT_Delete (StringTable_t* table)
{
    assert (table);

    StringEntry_t* cur = NULL;
    for (size_t i = 0; i < table->size; i++)
    {
        cur = table->buckets[i];
        while (cur)
        {
            StringEntry_t* next = cur->next;
            free (cur->string);
            free (cur);

            cur = next;
        }
    }
    return;
}
// ---------------------------------------------------------------------------------------------------
