#include <stdio.h>
#include <assert.h>

#include "ConfigLangConst.h"
#include "NodeFunc.h"


// ---------------------------------------------------------------------------------------------------
void writeCode (Node_t* node,
                int number_tab,
                FILE* stream)
{
    assert (node);
    assert (stream);

    if (node->type == NODE_TYPE_VAR)
    {
        fprintf (stream, "%s ", node->value.name);
    }
    else if (node->type == NODE_TYPE_FUNC)
    {
        fprintf (stream, "%s <", node->value.name);
        for (int i = 0; i < node->amount_children; i++)
        {
            fprintf (stream, "%s", node->children[i]->value.name);
            if (i != node->amount_children - 1)
                fprintf (stream, ", ");
            fprintf (stream, "> ");
        }
    }
    else if (node->type == NODE_TYPE_NUM)
    {
        fprintf (stream, "%d ", node->value.num);
    }
    else if (node->type == NODE_TYPE_OPER)
    {
        writeCode (node->children[0], number_tab, stream);
        for (size_t i = 0; sizeof (ALL_OPER_WORD) / sizeof (ALL_OPER_WORD[0]); i++)
        {
            if (node->value.oper == ALL_OPER_WORD[i].value)
            {
                fprintf (stream, "%s ", ALL_OPER_WORD[i].name);
                break;
            }
        }
        writeCode (node->children[1], number_tab, stream);
    }
    else if (node->type == NODE_TYPE_BLOCK)
    {
        fprintf (stream, "(\n");
        for (int i = 0; i < node->amount_children; i++)
        {
            writeCode (node->children[i], number_tab + 1, stream);
            fprintf (stream, "\n");
        }
        fprintf (stream, "%.*c)\n", number_tab, '\t');
    }
    else if (node->type == NODE_TYPE_KEY_WORD)
    {
        switch (node->value.key)
        {
        case (KEY_ASSIGN):
            fprintf (stream, "%.*c", number_tab, '\t');
            writeCode (node->children[0], number_tab, stream);
            fprintf (stream, "= ");
            writeCode (node->children[1], number_tab, stream);
            fprintf (stream, "\n");
            break;

        case (KEY_BREAK):
            fprintf (stream, "%.*cerr\n", number_tab, '\t');
            break;

        case (KEY_DEFINE_FUNC):
            fprintf (stream, "ser %s <", node->children[0]->value.name);
            for (int i = 1; i < node->amount_children - 1; i++)
            {
                fprintf (stream, "%s", node->children[i]->value.name);
                if (i != node->amount_children - 2)
                    fprintf (stream, ", ");
                fprintf (stream, "> ");
            }
            writeCode (node->children[node->amount_children - 1], stream, number_tab);
            break;

        case (KEY_DEFINE_VAR):
            fprintf (stream, "%.*cal ", number_tab, '\t');
            if (node->children[0]->type == NODE_TYPE_KEY_WORD)
            {
                fprintf (stream, "%s = %d\n", node->children[0]->children[0]->value.name,
                         node->children[0]->children[1]->value.num);
            }
            else
            {
                fprintf (stream, "%s = 0\n", node);
            }
            break;



        default:
            break;
        }
    }
}
// ---------------------------------------------------------------------------------------------------


