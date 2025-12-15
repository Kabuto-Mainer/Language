#include <stdio.h>
#include <assert.h>

#include "NodeType.h"
#include "ConfigTreeConst.h"
#include "SupportFunc.h"
#include "WriteTree.h"


// ---------------------------------------------------------------------------------------------------
void writeTree (Node_t* root)
{
    assert (root);

    FILE* stream = fopen (ADDRESS_TREE_FILE, "w");
    if (stream == NULL)
    {
        Log_Error("NULL file");
        return;
    }

    writeNode (root, stream, root->amount_children);
    fclose (stream);

    return;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи узла
 * \param [in] node Указатель на узел
 * \param [in] stream Файл для записи
 * \param [in] amount_last Количество детей полученного узла,
                           которых нужно записать
 */
void writeNode (Node_t* node,
                FILE* stream,
                int amount_last)
{
    assert (node);
    assert (stream);

    bool in_next = amount_last != node->amount_children;

    if (!in_next)
         writeValue (node, stream);

    if (amount_last == 0)
        fprintf (stream, "(.)(.)");

    else if (amount_last == 1)
    {
        writeNode (node->children[node->amount_children - amount_last],
                   stream,
                   node->children[node->amount_children - amount_last]->amount_children);
        fprintf (stream, "(.)");
    }
    else if (amount_last == 2)
    {
        writeNode (node->children[node->amount_children - amount_last],
                   stream,
                   node->children[node->amount_children - amount_last]->amount_children);
        writeNode (node->children[node->amount_children - amount_last + 1],
                   stream,
                   node->children[node->amount_children - amount_last + 1]->amount_children);
    }
    else
    {
        writeNode (node->children[node->amount_children - amount_last],
                   stream,
                   node->children[node->amount_children - amount_last]->amount_children);
        fprintf (stream, "(next:");
        writeNode (node, stream, amount_last - 1);
        fprintf (stream, ")");
    }

    if (!in_next)
        fprintf (stream, ")");

    return;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи типа и значения узла в файл
 * \param [in] node Указатель на узел
 * \param [in] stream Файл для записи
 * \note Открывает скобку '('
 */
void writeValue (Node_t* node,
                 FILE* stream)
{
    assert (node);
    assert (stream);

    fprintf (stream, "(");
    if (node->type == NODE_TYPE_VAR)
        fprintf (stream, "var:%s", node->val.name);
    else if (node->type == NODE_TYPE_NUM)
        fprintf (stream, "num:%d", node->val.num);
    else if (node->type == NODE_TYPE_FUNC)
        fprintf (stream, "func:%s", node->val.name);
    else if (node->type == NODE_TYPE_KEY_WORD)
    {
        fprintf (stream, "key:");
        for (size_t i = 0; i < sizeof (ALL_SYSTEM_WORD) / sizeof (ALL_SYSTEM_WORD[0]); i++)
        {
            if (ALL_SYSTEM_WORD[i].value == node->val.key)
            {
                fprintf (stream, "%s", ALL_SYSTEM_WORD[i].name);
                break;
            }
        }
    }
    else if (node->type == NODE_TYPE_OPER)
    {
        fprintf (stream, "oper:");
        for (size_t i = 0; i < sizeof (ALL_OPER_WORD) / sizeof (ALL_OPER_WORD[0]); i++)
        {
            if (ALL_OPER_WORD[i].value == node->val.oper)
            {
                fprintf (stream, "%s", ALL_OPER_WORD[i].name);
                break;
            }
        }
    }
    else if (node->type == NODE_TYPE_BLOCK)
    {
        fprintf (stream, "block:");
    }
    else
    {
        Log_Error ("Not correct type in tree");
        return;
    }
    return;
}
// ---------------------------------------------------------------------------------------------------
