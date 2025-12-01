#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "string.h"

#include "NameTableType.h"
#include "NameTableConfig.h"
#include "NameTableFunc.h"

#include "../Support/SupportFunc.h"

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция создания таблицы имен
 @param [in] table Указатель на структуру таблицы имен
*/
int nameTableCtr (NameTable_t* table)
{
    assert (table);

    table->data = (NameTableVar_t*) calloc (START_SIZE_TABLE, sizeof (NameTableVar_t));
    if (table->data == NULL)
    {
        EXIT_FUNC("NULL calloc", 1);
    }

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
        free (table->data[i].name);

    free (table->data);
    free (table);

    return 0;
}
// ---------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция поиска имени в таблице имен
 @param [in] table Указатель на структуру таблицы имен
 @param [in] name Полученное имя
 @return Индекс переменной с таким именем в таблице имен,
         если не найдет (size_t) -1
*/
size_t nameTableFind (NameTable_t* table,
                      NameTableName_t name)
{
    assert (table);
    assertNameTableName (name);

    size_t hash = getHash (name);
    NameTableVar_t* data = table->data;

    for (size_t i = 0; i < table->size; i++)
    {
        if (hash == data[i].hash && strcmp (name, data[i].name))
            return i;
    }
    return (size_t) -1;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция сравнения элементов таблицы имен по их хешам
 @param [in] var_1 Указатель на первый элемент
 @param [in] var_2 Указатель на второй элемент
 @return 0, если совпадают хеши и имена, 1 - если совпадают только хеши,
         иначе 1, если hash_1 > hash_2, 0 если hash_1 < hash_2
*/
int compareVar (const void* var_1,
                const void* var_2)
{
    assert (var_1);
    assert (var_2);

    NameTableVar_t* struct_var_1 = (NameTableVar_t*) var_1;
    NameTableVar_t* struct_var_2 = (NameTableVar_t*) var_2;

    if (struct_var_1->hash > struct_var_2->hash) { return 1; }
    if (struct_var_1->hash < struct_var_2->hash) { return -1; }

    return strcmp (struct_var_1->name, struct_var_2->name);
}
// ---------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция, возвращающая имя, лежащее под полученным индексом
 @param [in] table Указатель на таблицу имен
 @param [in] index Полученный индекс
 @return Имя, иначе NULL
*/
NameTableName_t nameTableGetName (const NameTable_t* table,
                                  const size_t index)
{
    assert (table);
    return table->data[index].name;
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
 @param [in] value Значение, соответствующее имени
 @param [in] len_name Длина имени
 @return Индекс имени в таблице имен (при ошибке (size_t) -1)
*/
size_t nameTableAdd (NameTable_t* table,
                     NameTableName_t name,
                     NameTableValue_t value,
                     size_t len_name)
{
    assert (table);
    assertNameTableName (name);

    if (table->size >= table->capacity)
    {
        NameTableVar_t* buffer = (NameTableVar_t*) realloc (table->data, table->capacity * 2);
        if (buffer == NULL)
            EXIT_FUNC("NULL realloc", -1);

        table->data = buffer;
        table->capacity *= 2;
    }

    table->data[table->size].name = strndup (name, len_name);
    table->data[table->size].value = value;
    table->data[table->size].hash = getHash (table->data[table->size].name);
    table->size++;
    // qsort (table->data, table->size, sizeof (NameTableVar_t), compareVar);
    return nameTableFind (table, name);
}
// ---------------------------------------------------------------------------------------------------



