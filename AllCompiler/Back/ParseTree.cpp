#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "NodeFunc.h"
#include "NodeType.h"
#include "ConfigTreeConst.h"
#include "TokenType.h"
#include "SupportFunc.h"
#include "ParseTree.h"


// ---------------------------------------------------------------------------------------------------
/// \brief Макрос обработки ошибок
#define SYNTAX_ERROR(__error__,__ret_val__)                         \
    do {                                                            \
        printf ("ERROR in tree from %s:%d\n\t", __FILE__, __LINE__);\
        parserError (__error__);                                    \
        Log_Error ("Error in tree (syntax)");                       \
        return __ret_val__;                                         \
    } while (0)
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \ Функция нахождения значения для команды по имени команды
 * \param [in] data Указатель на структуру хранения
 * \param [in] size Количество элементов в этой базе данных
 * \param [in] name Имя команды
 * \return Значение, соответствующее этой команде
*/
int findWordValue (const SystemSyntaxWord_t* data,
                    size_t size,
                    char* name)
{
    assert (data);
    assert (name);

    for (size_t i = 0; i < size; i++)
    {
        if (strcmp (data[i].name, name) == 0)
        {
            printf ("Name: %s\n", name);
            return data[i].value;
        }
    }
    return -1;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция чтения буфера и построения по нему дерева
 * \param [in] parent Указатель на родительский узел
 * \param [in] pose Указатель на файл дерева
 * \return Указатель на созданный узел
*/
Node_t* readNode (Node_t* parent,
                  char** pose)
{
    assert (parent);
    assert (pose);

    // TODO: сделать отдельную функцию
    // printf ("STR:%s:\n\n", *pose);
    if (**pose != '(')
        SYNTAX_ERROR (BACK_ERROR_NOT_LEFT_ROUND, NULL);

    ++ *pose;
    char type[MAX_LEN_NAME] = "";
    int len = 0;
    Node_t* cur_node = newNode ();

    if (sscanf (*pose, "%[^():]:%n", type, &len) != 1)
    {
        free (cur_node);
        SYNTAX_ERROR (BACK_ERROR_SCANF, NULL);
    }

    *pose += len;
    if      (strcmp (type, "key") == 0)
    {
        char name[MAX_LEN_NAME] = "";
        // ++ *pose; // Skip ':'

        if (sscanf (*pose, "%[^()]%n", name, &len) != 1)
        {
            free (cur_node);
            SYNTAX_ERROR (BACK_ERROR_KEY, NULL);
        }

        *pose += len;
        cur_node->type = NODE_TYPE_KEY_WORD;
        cur_node->value.key = findWordValue (ALL_SYSTEM_WORD,
            sizeof (ALL_SYSTEM_WORD) / sizeof (ALL_SYSTEM_WORD[0]), name);
    }
    else if (strcmp (type, "var") == 0)
    {
        char name[MAX_LEN_NAME] = "";
        // ++ *pose;

        if (sscanf (*pose, "%[^()]%n", name, &len) != 1)
        {
            free (cur_node);
            SYNTAX_ERROR (BACK_ERROR_VAR, NULL);
        }

        *pose += len;
        cur_node->type = NODE_TYPE_VAR;
        cur_node->value.name = strdup (name);
    }
    else if (strcmp (type, "func") == 0)
    {
        char name[MAX_LEN_NAME] = "";
        if (sscanf (*pose, "%[^()]%n", name, &len) != 1)
        {
            free (cur_node);
            SYNTAX_ERROR (BACK_ERROR_VAR, NULL);
        }
        *pose += len;
        cur_node->type = NODE_TYPE_FUNC;
        cur_node->value.name = strdup (name);
    }
    else if (strcmp (type, "oper") == 0)
    {
        char name[MAX_LEN_NAME] = "";
        // ++ *pose;

        if (sscanf (*pose, "%[^()]%n", name, &len) != 1)
        {
            free (cur_node);
            SYNTAX_ERROR (BACK_ERROR_OPER, NULL);
        }

        *pose += len;
        cur_node->type = NODE_TYPE_OPER;
        cur_node->value.oper = findWordValue (ALL_OPER_WORD,
            sizeof(ALL_OPER_WORD) / sizeof(ALL_OPER_WORD[0]), name);
    }
    else if (strcmp (type, "num") == 0)
    {
        int val = 0;
        // ++ *pose;

        if (sscanf (*pose, "%d%n", &val, &len) != 1)
        {
            free (cur_node);
            SYNTAX_ERROR (BACK_ERROR_NUM, NULL);
        }

        *pose += len;
        cur_node->type = NODE_TYPE_NUM;
        cur_node->value.num = val;
    }
    else if (strcmp (type, "block") == 0)
    {
        // ++ *pose;
        cur_node->type = NODE_TYPE_BLOCK;
        cur_node->value.name = NULL;
    }
    else if (strcmp (type, "next") == 0)
    {
        free (cur_node);
        cur_node = parent;
    }
    else if (strcmp (type, ".") == 0)
    {
        free (cur_node);
        *pose += 2; // Skip '.)'
        // printf ("STR in ...: %s\n", *pose);
        return NULL;
    }
    else
    {
        free (cur_node);
        SYNTAX_ERROR (BACK_ERROR_UNKNOWN_TYPE, NULL);
    }

    if (strcmp (type, "next") != 0)
        addNode (parent, cur_node);

    // printf ("TYPE %s\n", type);
    // printf ("STR::::%s\n", *pose);
    readNode (cur_node, pose); // Left
    readNode (cur_node, pose); // Right

    if (**pose != ')')
        SYNTAX_ERROR (BACK_ERROR_NOT_RIGHT_ROUND, NULL);

    ++ *pose;
    return cur_node;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция вывода описания ошибки по её номеру
 * \param [in] error Номер ошибки
 */
void parserError (BACK_ERRORS error)
{
    switch (error)
    {
        case (BACK_ERROR_NOT_LEFT_ROUND):
            printf ("Missing left bracket '('\n");
            break;
        case (BACK_ERROR_NOT_RIGHT_ROUND):
            printf ("Missing right bracket ')'\n");
            break;
        case (BACK_ERROR_SCANF):
            printf ("Incorrect scanf type\n");
            break;
        case (BACK_ERROR_NUM):
            printf ("Incorrect scanf number\n");
            break;
        case (BACK_ERROR_OPER):
            printf ("Incorrect scanf operation\n");
            break;
        case (BACK_ERROR_VAR):
            printf ("Incorrect scanf variable\n");
            break;
        case (BACK_ERROR_KEY):
            printf ("Incorrect scanf key word\n");
            break;
        case (BACK_ERROR_UNKNOWN_TYPE):
            printf ("Unknown type in tree\n");
            break;
        default: {}
    }
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Рекурсивная функция удаления узлов поддерева
 * \param [in] node Указатель на корень поддерева
 */
void deleteNode (Node_t* node)
{
    assert (node);

    for (int i = 0; i < node->amount_children; i++)
        deleteNode (node->children[i]);

    if ((node->type == NODE_TYPE_FUNC ||
         node->type == NODE_TYPE_VAR  ||
         node->type == NODE_TYPE_BLOCK) &&
        node->value.name != NULL)
        free (node->value.name);

    if (node->amount_children != 0)
        free (node->children);

    free (node);
}
// ---------------------------------------------------------------------------------------------------

