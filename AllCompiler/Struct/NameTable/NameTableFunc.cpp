#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "string.h"

#include "NameTableConfig.h"
#include "NameTableFunc.h"
#include "SupportFunc.h"


// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция создания таблицы имен
 @param [in] table Указатель на структуру таблицы имен
*/
int nameTableCtr (NameTable_t* table)
{
    assert (table);

    // table->data = NULL;
    table->data = (NameTableVar_t*) calloc (START_SIZE_TABLE, sizeof (NameTableVar_t));
    if (table->data == NULL)
        EXIT_FUNC("NULL calloc", 1);

    table->capacity = START_SIZE_TABLE;
    table->size = 0;

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция удаления структуры таблицы имен
 @param [in] table Указатель на структуру таблицы имен
*/
int nameTableDtr (NameTable_t* table)
{
    assert (table);

    for (size_t i = 0; i < table->size; i++)
    {
        NameTableVar_t* buffer = &(table->data[i]);
        // printf ("S: %s\n", buffer->name);
        free (buffer->name);
        if (buffer->next == NULL)
        {
            continue;
        }
        buffer = buffer->next;
        while (buffer != NULL)
        {
            NameTableVar_t* next_buffer = buffer->next;
            free (buffer->name);
            free (buffer);
            buffer = next_buffer;
        }
    }

    if (table->data != NULL)
        free (table->data);
    // free (table);

    return 0;
}
// ---------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция поиска имени в таблице имен
 @param [in] table Указатель на структуру таблицы имен
 @param [in] name Полученное имя
 @return Указатель на структуру таблицы переменных, если найдена,
         иначе NULL
*/
NameTableVar_t* nameTableFind (NameTable_t* table,
                               char* name)
{
    assert (table);
    assertNameTableName (name);

    size_t hash = getHash (name);
    NameTableVar_t* data = table->data;

    NameTableVar_t* var = (NameTableVar_t*) bsearch (&hash, data,
                          table->size, sizeof (NameTableVar_t), compareVarAndHash);

    if (var == NULL)
        return NULL;

    // printf ("FIND: %s\nINPUT: %s\n", var->name, name);
    while (var != NULL)
    {
        if (strcmp (var->name, name) == 0)
            return var;
        var = var->next;
    }
    return NULL;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция сравнения элементов таблицы по хешу
 @param [in] void_var_1 Указатель на первый элемент
 @param [in] void_var_2 Указатель на второй элемент
 @return sign (var_1.hash - var_2.hash)
*/
int compareVarByHash (const void* void_var_1,
                      const void* void_var_2)
{
    assert (void_var_1);
    assert (void_var_2);

    const NameTableVar_t* var_1 = (const NameTableVar_t*) void_var_1;
    const NameTableVar_t* var_2 = (const NameTableVar_t*) void_var_2;

    if (var_1->hash > var_2->hash)  { return 1; }
    if (var_1->hash < var_2->hash)  { return -1; }
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция сравнения хеша и элемента таблицы по хешу
 @param [in] void_hash_1 Указатель на хеш
 @param [in] void_var_2 Указатель на элемент
 @return sign (hash_1 - var_2.hash)
*/
int compareVarAndHash (const void* void_hash_1,
                       const void* void_var_2)
{
    assert (void_hash_1);
    assert (void_var_2);

    const size_t* hash_1 = (const size_t*) void_hash_1;
    const NameTableVar_t* var_2 = (const NameTableVar_t*) void_var_2;

    if (*hash_1 > var_2->hash)  { return 1; }
    if (*hash_1 < var_2->hash)  { return -1; }
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция получения текущего размера таблицы имен
 @param [in] table Указатель на структуру таблицы
 @return Размер
*/
size_t nameTableGetSize (NameTable_t* table)
{
    assert (table);
    return table->size;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция добавления элемента в таблицу имен
 @param [in] table Указатель на таблицу имен
 @param [in] name Имя добавляемого объекта
 @return Указатель на элемент
*/
NameTableVar_t* nameTableAdd (NameTable_t* table,
                              char* name)
{
    assert (table);
    assertNameTableName (name);

    NameTableVar_t* find_var = nameTableFind (table, name);
    if (find_var != NULL)
        return find_var;

    size_t hash = getHash (name);
    NameTableVar_t* data = table->data;

    find_var = (NameTableVar_t*) bsearch (&hash, data, table->size,
                                 sizeof (NameTableVar_t), compareVarAndHash);

    // printf ("ADR: %p\n", find_var);
    if (find_var == NULL)
    {
        if (table->size >= table->capacity)
        {
            NameTableVar_t* buffer = (NameTableVar_t*) realloc (table->data, table->capacity * 2);
            if (buffer == NULL)
                EXIT_FUNC ("NULL realloc", NULL);

            table->data = buffer;
            table->capacity *= 2;
        }
        table->data[table->size].name = strdup (name);
        table->data[table->size].hash = hash;
        table->data[table->size].next = NULL;
        table->size++;
        qsort (table->data, table->size, sizeof (NameTableVar_t), compareVarByHash);
        find_var = nameTableFind (table, name);
    }
    else
    {
        NameTableVar_t* buffer_var = find_var;
        int size = 1;
        while (buffer_var->next != NULL)
        {
            size++;
            buffer_var = buffer_var->next;
        }
        // printf ("SIZE: %d\n", size);

        find_var = (NameTableVar_t*) calloc (1, sizeof (NameTableVar_t));
        if (find_var == NULL)
            EXIT_FUNC ("NULL calloc", NULL);

        buffer_var->next = find_var;
        find_var->next = NULL;
        find_var->hash = hash;
        find_var->name = strdup (name);

        // printf ("NAME: %s\n", find_var->name);
    }
    return find_var;
}
// ---------------------------------------------------------------------------------------------------



// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция создания структуры стека
 @param [in] stack Указатель на структуру стека
*/
int nameTableStackCtr (NameTableStack_t* stack)
{
    assert (stack);

    stack->size = 0;
    stack->capacity = START_SIZE_STACK;
/*
    if (START_SIZE_STACK != 0)
    {
    }
*/
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция пуша таблицы имен в стек
 @param [in] stack Указатель на структуру стека
 @param [in] table Указатель на таблицу имен
*/
int nameTableStackPush (NameTableStack_t* stack,
                        NameTable_t* table)
{
    assert (stack);
    assert (table);

    NameTable_t** buffer = stack->data;
    if (stack->capacity == 0)
    {
        buffer = (NameTable_t**) calloc (2, sizeof (NameTable_t*));
        if (buffer == NULL)
            EXIT_FUNC ("NULL calloc", 1);
        stack->capacity = 2;
    }
    else if (stack->size >= stack->capacity)
    {
        buffer = (NameTable_t**) realloc (stack->data, sizeof (NameTable_t*) * stack->capacity * 2);
        if (buffer == NULL)
            EXIT_FUNC ("NULL calloc", 1);

        stack->capacity *= 2;
    }
    stack->data = buffer;
    stack->data[stack->size++] = table;
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция POPа элемента из стека
 @param [in] stack Указатель на структуру стека
 @return Элемент, который POPнули
*/
NameTable_t* nameTableStackPop (NameTableStack_t* stack)
{
    assert (stack);

    if (stack->size == 0)
        EXIT_FUNC ("Pop with size = 0", NULL);

    return stack->data[--stack->size];
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция, возвращающая элемент под переданным индексом
 @param [in] stack Указатель на структуру стека
 @param [in] index Переданный индекс
 @return Элемент, если успешно, иначе NULL
*/
NameTable_t* nameTableStackGetEl (NameTableStack_t* stack,
                                  size_t index)
{
    assert (stack);

    if (stack->size < index)
        return NULL;

    return stack->data[index];
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция удаления стека
 @param [in] stack Укащатель на структуру стека
*/
int nameTableStackDtr (NameTableStack_t* stack)
{
    assert (stack);

    for (size_t i = 0; i < stack->size; i++)
        nameTableDtr (stack->data[i]);

    if (stack->data != NULL)
        free (stack->data);

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция получения размера стека
 @param [in] stack Указатель на структуру стека
 @return Размер стека
*/
size_t nameTableStackGetSize (NameTableStack_t* stack)
{
    return stack->size;
}
// ---------------------------------------------------------------------------------------------------

