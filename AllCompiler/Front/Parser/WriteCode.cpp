#include <stdio.h>
#include <assert.h>

#include "ConfigLangConst.h"
#include "NodeType.h"
#include "SupportFunc.h"
#include "WriteCode.h"


// ---------------------------------------------------------------------------------------------------
static char TABS[32] = "";
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи дерева в формате языка в файл
 * \param [in] root Указатель на корень дерева
 * \param [in] name_file Имя файла, куда будет производиться запись
 */
void writeCodeToFile (Node_t* root,
                      const char* name_file)
{
    assert (root);
    assert (name_file);

    // printf ("ROOT %p\n", root);
    FILE* stream = fopen (name_file, "w");
    if (stream == NULL)
        EXIT_FUNC ("NULL file", );

    for (int i = 0; i < (int) (sizeof (TABS) / sizeof (TABS[0])); i++)
        TABS[i] = '\t';

    TABS[sizeof (TABS) / sizeof (TABS[0]) - 1] = '\0';
    for (int i = 0; i < root->amount_children; i++)
    {
        writeCode (root->children[i], 0, stream);
    // fprintf (stream, "sdjhbsdjbdbxnvldsjbjdboasjblfkjbsldkfbalksjdbflksjdblksjadb\n");
    }
    fclose (stream);
    return;
}
// ---------------------------------------------------------------------------------------------------



// TODO: create new function
// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи поддерева в файл
 * \param [in] node Указатель на корень поддерева
 * \param [in] number_tab Количество TAB, которое нужно поставить перед выводимой точкой
 * \param [in] stream Указатель на файл
 */
void writeCode (Node_t* node,
                int number_tab,
                FILE* stream)
{
    assert (node);
    assert (stream);

    if (node->type == NODE_TYPE_VAR)
    {
        // printf ("VAR\n");
        fprintf (stream, "%s", node->value.name);
    }
    else if (node->type == NODE_TYPE_FUNC)
    {
        // printf ("FUNC (%p %d)\n", node, node->amount_children);
        fprintf (stream, "%s <", node->value.name);
        for (int i = 0; i < node->amount_children; i++)
        {
            writeCode (node->children[i], number_tab, stream);
        }
        fprintf (stream, "> ");
    }
    else if (node->type == NODE_TYPE_NUM)
    {
        // printf ("NUM\n");
        fprintf (stream, "%d", node->value.num);
    }
    else if (node->type == NODE_TYPE_OPER)
    {
        // printf ("OPER\n");
        writeCode (node->children[0], number_tab, stream);
        for (size_t i = 0; i < sizeof (ALL_OPER_WORD) / sizeof (ALL_OPER_WORD[0]); i++)
        {
            if (node->value.oper == ALL_OPER_WORD[i].value)
            {
                fprintf (stream, " %s ", ALL_OPER_WORD[i].name);
                break;
            }
        }
        writeCode (node->children[1], number_tab, stream);
    }
    else if (node->type == NODE_TYPE_BLOCK)
    {
        // printf ("BLOCK\n");
        fprintf (stream, "(\n");
        for (int i = 0; i < node->amount_children; i++)
        {
            writeCode (node->children[i], number_tab + 1, stream);
            // fprintf (stream, "\n");
        }
        fprintf (stream, "%.*s)\n", number_tab, TABS);
    }
    else if (node->type == NODE_TYPE_KEY_WORD)
    {
        // printf ("KEY\n");
        // printf ("slkjfdsljhbfksjhbfjs\n");
        switch (node->value.key)
        {
        case (KEY_ASSIGN):
            fprintf (stream, "%.*s", number_tab, TABS);
            writeCode (node->children[0], number_tab, stream);
            fprintf (stream, " = ");
            writeCode (node->children[1], number_tab, stream);
            fprintf (stream, "\n");
            break;

        case (KEY_BREAK):
            fprintf (stream, "%.*serr\n", number_tab, TABS);
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
            writeCode (node->children[node->amount_children - 1], number_tab, stream);
            break;

        case (KEY_DEFINE_VAR):
            fprintf (stream, "%.*sal ", number_tab, TABS);
            if (node->children[0]->type == NODE_TYPE_KEY_WORD)
            {
                fprintf (stream, "%s = %d\n", node->children[0]->children[0]->value.name,
                         node->children[0]->children[1]->value.num);
            }
            else
            {
                fprintf (stream, "%s = 0\n", node->children[0]->value.name);
            }
            break;

        case (KEY_IF):
        case (KEY_ELSE_IF):
            if (node->value.key == KEY_IF)
                fprintf (stream, "%.*str <", number_tab, TABS);
            else
                fprintf (stream, "%.*sntr <", number_tab, TABS);
            writeCode (node->children[0], number_tab, stream);
            fprintf (stream, "> ");
            writeCode (node->children[1], number_tab, stream);
            break;

        case (KEY_ELSE):
            fprintf (stream, "%.*sdef ", number_tab, TABS);
            writeCode (node->children[0], number_tab, stream);
            break;

        case (KEY_IN):
        case (KEY_OUT):
            if (node->value.key == KEY_IN)
                fprintf (stream, "%.*sin <", number_tab, TABS);
            else
                fprintf (stream, "%.*sout <", number_tab, TABS);
            writeCode (node->children[0], number_tab, stream);
            fprintf (stream, ">\n");
            break;

        case (KEY_RETURN):
            fprintf (stream, "%.*sret ", number_tab, TABS);
            writeCode (node->children[0], number_tab, stream);
            fprintf (stream, "\n");
            break;

        case (KEY_WHILE):
            fprintf (stream, "%.*sloop <", number_tab, TABS);
            writeCode (node->children[0], number_tab, stream);
            fprintf (stream, "> ");
            writeCode (node->children[1], number_tab, stream);
            break;

        default:
            break;
        }
    }
    fflush (stream);
}
// ---------------------------------------------------------------------------------------------------


