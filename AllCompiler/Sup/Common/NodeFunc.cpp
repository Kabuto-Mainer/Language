#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "SupportFunc.h"
#include "NodeFunc.h"
#include "TypesOfType.h"


// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция добавления детей node_2 к детям node_1
 @param [in] node_1 К чьим детям пойдет прибавление
 @param [in] node_2 Чьих детей добавляют
*/
int addChildren (Node_t* node_1,
                 Node_t* node_2)
{
    assert (node_1);
    assert (node_2);

    for (int i = 0; i < node_2->amount_children; i++)
        addNode (node_1, node_2->children[i]);

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция добавления child в дети parent
 @param [in] parent Кому добавляются дети
 @param [in] child Кого добавляют
*/
int addNode (Node_t* parent,
             Node_t* child)
{
    assert (parent);
    assert (child);

    Node_t** buffer = NULL;
    if (parent->children == NULL)
    {
        buffer = (Node_t**) calloc (1, sizeof (Node_t*));
        if (buffer == NULL)
            EXIT_FUNC("NULL calloc", 1);
        parent->amount_children = 0;
    }
    else
    {
        buffer = (Node_t**) realloc (parent->children,
                  (size_t) (parent->amount_children + 1) * sizeof (Node_t*));
        if (buffer == NULL)
            EXIT_FUNC("NULL realloc", 1);
    }
    buffer[parent->amount_children] = child;
    child->parent = parent;
    parent->amount_children++;
    parent->children = buffer;

    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция выделения динамической памяти под узел
 @return Указатель на выделенную память
*/
Node_t* newNode ()
{
    Node_t* node = (Node_t*) calloc (1, sizeof (Node_t));
    if (node == NULL)
        EXIT_FUNC ("NULL calloc", NULL);
    return node;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция удаления буферного элемента
 @param [in] node Указатель на узел, который будет удален
 @note Удаляет только память узла и САМ УЗЕЛ (он должен быть выделен через дин. память)
*/
int deleteOneNode (Node_t* node)
{
    assert (node);
    free (node->children);
    free (node);
    return 0;
}
// ---------------------------------------------------------------------------------------------------
