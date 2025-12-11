#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "SupportFunc.h"
#include "VectorFunc.h"
#include "VectorConfig.h"

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция создания динамического массива с авто расширением
 * \param [in] vector Указатель на структуру вектора
*/
int vectorCtr (TokenVector_t* vector)
{
    assert (vector);

    Node_t* data = (Node_t*) calloc (VECTOR_START_CAPACITY, sizeof (Node_t));
    if (data == NULL)
        EXIT_FUNC ("NULL calloc", 1);

    vector->capacity = VECTOR_START_CAPACITY;
    vector->size = 0;
    vector->data = data;

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция добавления элемента в вектор
 * \param [in] vector Указатель на структуру вектора
 * \param [in] token Добавляемый элемент
*/
int vectorAdd (TokenVector_t* vector,
               const VectorVar_t token)
{
    assert (vector);

    // printf ("SIZE %zu\nCAP %zu\n", vector->size, vector->capacity);
    if (vector->size >= vector->capacity)
    {
        VectorVar_t* buffer = (VectorVar_t*) realloc (vector->data,
                              vector->capacity * 2 * sizeof (VectorVar_t));
        if (buffer == NULL)
            EXIT_FUNC ("NULL realloc", 1);
        vector->data = buffer;
        vector->capacity *= 2;
    }
    vector->data[vector->size] = token;
    vector->size++;
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция получения текущего количества элементов в векторе
 * \param [in] vector Указатель на структуру вектора
 * \return Количество элементов
*/
size_t vectorGetSize (TokenVector_t* vector)
{
    assert (vector);
    return vector->size;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция получения элемента вектора по его индексу
 * \param [in] vector Указатель на структуру вектора
 * \param [in] index Индекс элемента
 * \return Указатель на элемента в случае успеха, иначе NULL
*/
VectorVar_t* vectorGetToken (TokenVector_t* vector,
                             size_t index)
{
    assert (vector);

    if (index >= vector->size)
        return NULL;

    return vector->data + index;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция удаления памяти вектора
 * \param [in] vector Указатель на структуру вектора
*/
int vectorDtr (TokenVector_t* vector)
{
    assert (vector);

    for (size_t i = 0; i < vector->size; i++)
        vectorDtrToken (&(vector->data[i]));

    free (vector->data);
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция удаления элемента вектора
 * \param [in] token Указатель на элемент
*/
int vectorDtrToken (VectorVar_t* token)
{
    assert (token);
    if ((token->type == NODE_TYPE_INDENT ||
         token->type == NODE_TYPE_VAR ||
         token->type == NODE_TYPE_FUNC ||
         token->type == NODE_TYPE_BLOCK) &
         (token->value.name != NULL))
    {
        // printf ("STR: %s\n", token->value.name);
        free (token->value.name);
    }

    if (token->children != NULL)
        free (token->children);
    return 0;
}
// ---------------------------------------------------------------------------------------------------
