#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "DumpFunc.h"
#include "TypesOfType.h"
#include "SupportFunc.h"
#include "DumpConfig.h"
#include "TypesConst.h"
#include "TokenFunc.h"
#include "ConfigLangDump.h"


// -------------------------------------------------------------------------------------------------------
static int AMOUNT_IMAGES = 0;
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
/**
 @brief Функция дампа дерева
 @param [in] root Корень дерева
 @param [in] reason Причина дампа
*/
int treeDump (Node_t* root,
              const char* reason)
{
    assert (root);

    createGraph (root);
    FILE* html_file = fopen (DUMP_ADDRESS_HTML, "a");
    if (html_file == NULL)
    {
        EXIT_FUNC ("NULL file", 1);
    }

    char comand[200] = "";
    sprintf (comand, "dot %s -T png -o %simg%d.png",
             DUMP_ADDRESS_DOT,
             DUMP_ADDRESS_IMG,
             AMOUNT_IMAGES);

    int trash = system (comand);
    (void) trash;

    fprintf (html_file, "\n* Dump *\n");
    if (reason == NULL)     { fprintf (html_file, "No reason to dump (standard dump)\n"); }
    else                    { fprintf (html_file, "Reason: %s\n", reason); }

    fprintf (html_file,
            "<img src=Image/img%d.png width=%dpx>\n",
            AMOUNT_IMAGES,
            10000);
    AMOUNT_IMAGES++;

    fclose (html_file);
    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
/**
 @brief Функция создания графа по дереву
 @param [in] root Корень дерева
 @return 0 (если не было ошибок), иначе 1
*/
int createGraph (Node_t* root)
{
    assert (root);

    FILE* file = fopen (DUMP_ADDRESS_DOT, "w");
    if (file == NULL)
    {
        EXIT_FUNC ("NULL file", 1);
    }

    fprintf (file,
             "digraph {\n"
             "  rankdir=UD;\n"
             "  bgcolor=\"#1e1e1e\"\n"
             "  splines=spline;\n"
             "  nodesep=0.4;\n"
             "  ranksep=0.6;\n"
             "  node [shape=plaintext, style=filled, fontname=\"Helvetica\"];\n"
             "  edge [arrowhead=vee, arrowsize=0.6, penwidth=1.2];\n\n");

    createBlock (root, file);
    createLine (root, file);

    fprintf (file, "\n}\n");
    fclose (file);
    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
/**
 @brief Рекурсивная функция создания блоков графа
 @param [in] node Указатель на текущий узел
 @param [in] stream Указатель на dot файл
*/
int createBlock (Node_t* node,
                 FILE* stream)
{
    assert (node);
    assert (stream);

    for (int i = 0; i < node->amount_children; i++)
        createBlock (node->children[i], stream);

    printFullBlock (node, stream);

    return 0;
}
// -------------------------------------------------------------------------------------------------------

#ifndef DEBUG_MODE
// ------------------------------------------------------------------------------------------------------
/**
 @brief Функция описания блока дерева для кратного дампа
 @param [in] node Узел, блок которого описывается
 @param [in] stream Файл для вывода
*/
int printFullBlock (Node_t* node,
                    FILE* stream)
{
    assert (node);
    assert (stream);

    char shape[20] = "box";
    char color[10] = "";
    char label[40] = "";

    // dumpNode (node);
    // printf ("%d\n", node->type);
    if (node->type == NODE_TYPE_FUNC)
    {
        strcpy (color, "#4fdc36ff");
        sprintf (label, "%s", node->value.name);
    }
    else if (node->type == NODE_TYPE_INDENT)
    {
        strcpy (color, "#1bc3a7ff");
        // if (node->value.name != NULL)
            sprintf (label, "%s", node->value.name);
    }
    else if (node->type == NODE_TYPE_VAR)
    {
        strcpy (color, "#c1f453ff");
        sprintf (label, "%s", node->value.name);
    }
    else if (node->type == NODE_TYPE_KEY_WORD)
    {
        strcpy (color, "#0f32e0ff");
        sprintf (label, "%s", CHAR_KEY_WORD[node->value.key]);
    }
    else if (node->type == NODE_TYPE_PUNCT)
    {
        strcpy (color, "#ddf331ff");
        sprintf (label, "%s", CHAR_PUNCT[node->value.punct]);
    }
    else if (node->type == NODE_TYPE_NUM)
    {
        strcpy (color, "#de3434ff");
        sprintf (label, "%d", node->value.num);
    }
    else if (node->type == NODE_TYPE_OPER)
    {
        strcpy (color, "#e8802bff");
        sprintf (label, "%s", CHAR_OPER_TYPE[node->value.oper]);
    }
    else
    {
        strcpy (color, "#d119ffff");
        sprintf (label, "block");
    }

    fprintf (stream, "block_%p [shape=%s, style=filled, "
             "fillcolor=\"%s\", label=\"%s\"];\n",
             node, shape, color, label);
    return 0;
}
// -------------------------------------------------------------------------------------------------------
#else
// ------------------------------------------------------------------------------------------------------
/**
 @brief Функция описания блока дерева для кратного дампа
 @param [in] node Узел, блок которого описывается
 @param [in] stream Файл для вывода
*/
int printFullBlock (Node_t* node,
                    FILE* stream)
{
    assert (node);
    assert (stream);

    char shape[] = "box";
    char type[20] = "";
    char color[10] = "";
    char label[40] = "";
    char func_ret_value[400] = "";

    if (node->type == NODE_TYPE_FUNC)
    {
        strcpy (color, "#4fdc36ff");
        sprintf (label, "\"%s\"", node->val.func.name);
        strcpy (type, "FUNC");

        if (node->val.func.ret_type != NULL)
        {
            sprintf (func_ret_value, "<TR><TD BGCOLOR=\"#386de7ff\" "
                     "COLSPAN=\"1\">TYPE RETURN VALUE</TD>"
                     "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%s</TD></TR>\n"
                     "<TR><TD BGCOLOR=\"#386de7ff\" "
                     "COLSPAN=\"1\">POINT LEVEL </TD>"
                     "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%d</TD></TR>\n",
                     node->val.func.ret_type, node->val.func.ret_point_level);
            // sprintf (func_ret_value, "<TR><TD BGCOLOR=\"#386de7ff\""
            //          "COMSPAN=\"1\" POINT LEVEL </TD>"
            //          "<TD BGCOLOR=\"#386de7ff\" COMSPAN=\"1\"%d</TD></TR>\n",
            //          node->val.func.ret_point_level);
        }
    }
    else if (node->type == NODE_TYPE_INDENT)
    {
        strcpy (color, "#1bc3a7ff");
        sprintf (label, "\"%s\"", node->val.name);
        strcpy (type, "INDENT");
    }
    else if (node->type == NODE_TYPE_VAR)
    {
        strcpy (color, "#c1f453ff");
        sprintf (label, "\"%s\"", node->val.name);
        strcpy (type, "VAR");

        if (node->val.var.type != NULL)
        {
            sprintf (func_ret_value, "<TR><TD BGCOLOR=\"#386de7ff\" "
                     "COLSPAN=\"1\">TYPE VAR</TD>"
                     "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%s</TD></TR>\n"
                     "<TR><TD BGCOLOR=\"#386de7ff\" "
                     "COLSPAN=\"1\">POINT LEVEL </TD>"
                     "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%d</TD></TR>\n",
                     node->val.var.type, node->val.var.point_level);
        }
    }
    else if (node->type == NODE_TYPE_KEY_WORD)
    {
        // tokenOneDump (node, "Key Dump");
        strcpy (color, "#0f32e0ff");
        for (size_t i = 0; i < sizeof (ALL_SYSTEM_WORD) / sizeof (ALL_SYSTEM_WORD[0]); i++)
        {
            if (node->val.key == ALL_SYSTEM_WORD[i].value)
            {
                sprintf (label, "\"%s\"", ALL_SYSTEM_WORD[i].name);
                break;
            }
        }
        strcpy (type, "KEY");
    }
    else if (node->type == NODE_TYPE_PUNCT)
    {
        // tokenOneDump (node, "Punct Dump");
        strcpy (color, "#ddf331ff");
        for (size_t i = 0; i < sizeof (ALL_PUNCT_WORD) / sizeof (ALL_PUNCT_WORD[0]); i++)
        {
            if (node->val.punct == ALL_PUNCT_WORD[i].value)
            {
                sprintf (label, "\"%s\"", ALL_PUNCT_WORD[i].name);
                break;
            }
        }
        strcpy (type, "PUNCT");
    }
    else if (node->type == NODE_TYPE_NUM)
    {
        strcpy (color, "#de3434ff");
        sprintf (label, "%d", node->val.num);
        strcpy (type, "NUM");
    }
    else if (node->type == NODE_TYPE_OPER)
    {
        // tokenOneDump (node, "Oper Dump");
        strcpy (color, "#e8802bff");
        for (size_t i = 0; i < sizeof (ALL_OPER_WORD) / sizeof (ALL_OPER_WORD[0]); i++)
        {
            if (node->val.oper.val == ALL_OPER_WORD[i].value)
            {
                sprintf (label, "\"%s\"", ALL_OPER_WORD[i].name);
                break;
            }
        }
        strcpy (type, "OPER");
    }
    else
    {
        strcpy (color, "#d119ffff");
        sprintf (label, "block");
        sprintf (type, "block");
    }

    fprintf (stream, "block_%p [shape=%s, label=<\n<TABLE CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
            "<TR><TD PORT=\"root\" BGCOLOR=\"%s\" COLSPAN=\"2\"><B>%p", node, shape, color, node);
    fprintf (stream, "</B></TD></TR>\n"
             "<TR><TD BGCOLOR=\"#b209ccff\" COLSPAN=\"2\">ROOT=%p", node->parent);
    fprintf (stream, "</TD></TR>\n"
             "<TR><TD BGCOLOR=\"#6bd934ff\" COLSPAN=\"2\">%s</TD></TR>\n"
             "<TR><TD BGCOLOR=\"#46f2f5ff\" COLSPAN=\"2\">%s", type, label);
    fprintf (stream, "</TD></TR>\n%s"
             "<TR>\n<TD PORT=\"child\" BGCOLOR=\"#ff7301ff\" COLSPAN=\"2\">CHILDREN [%d]</TD>",
             func_ret_value, node->amount_children);
    fprintf (stream, "</TR>\n</TABLE> >];\n\n");

    return 0;

}
// ------------------------------------------------------------------------------------------------------
#endif /* DEBUG_MODE */
// -------------------------------------------------------------------------------------------------------
/**
 @brief Функция распечатки адреса в HEX-формате
 @param [in] stream Файл для вывода
 @param [in] address Полученный адрес
*/
int printAddress (FILE* stream,
                  const void* address)
{
    assert (stream);

    if (address == NULL)
    {
        fprintf (stream, "NIL");
        return 0;
    }
    fprintf (stream, "%p", address);

    // const unsigned char* bytes = (const unsigned char*) &address;
    // size_t size = sizeof (address);

    // for (size_t i = 0; i < size; i++)     { fprintf (stream, "%X", bytes[i]); }

    return 0;
}
// -------------------------------------------------------------------------------------------------------
#ifndef DEBUG_MODE
// -------------------------------------------------------------------------------------------------------
/**
 @brief Рекурсивная функция создания связей между блоками
 @param [in] node Указатель на текущий узел
 @param [in] stream Указатель на dot файл
*/
int createLine (Node_t* node,
                FILE* stream)
{
    assert (node);
    assert (stream);

    for (int i = 0; i < node->amount_children; i++)
        createLine (node->children[i], stream);

    for (int i = 0; i < node->amount_children; i++)
    {
        Node_t* child = node->children[i];
        if (child->parent == node)
            fprintf (stream, "block_%p -> block_%p [color=\"#fefefeff\", "
            "penwidth = 1.5, arrowsize = 0.6, constraint = true, dir = both];\n",
                     node, child);

        else
            fprintf (stream, "block_%p -> block_%p [color=\"#f82f2fff\", "
            "penwidth = 1.5, arrowsize = 0.6, constraint = true, dir = both];\n",
                     node, child);
    }
    return 0;
}
// -------------------------------------------------------------------------------------------------------
#else
// -------------------------------------------------------------------------------------------------------
/**
 @brief Рекурсивная функция создания связей между блоками
 @param [in] node Указатель на текущий узел
 @param [in] stream Указатель на dot файл
*/
int createLine (Node_t* node,
                FILE* stream)
{
    assert (node);
    assert (stream);

    for (int i = 0; i < node->amount_children; i++)
        createLine (node->children[i], stream);


    for (int i = 0; i < node->amount_children; i++)
    {
        Node_t* child = node->children[i];
        if (child->parent == node)
            fprintf (stream, "block_%p:child -> block_%p:root [color=\"#fefefeff\", "
            "penwidth = 1.5, arrowsize = 0.6, constraint = true, dir = both];\n",
                     node, child);

        else
            fprintf (stream, "block_%p:child -> block_%p:root [color=\"#f82f2fff\", "
            "penwidth = 1.5, arrowsize = 0.6, constraint = true, dir = both];\n",
                     node, child);
    }

    return 0;
}
// -------------------------------------------------------------------------------------------------------
#endif /* DEBUG_MODE */
// -------------------------------------------------------------------------------------------------------
int dumpNode (Node_t* node)
{
    assert (node);

    printf ("[%p]\nAM: %d\n", node, node->amount_children);
    for (int i = 0; i < node->amount_children; i++)
    {
        printf ("    %p\n", node->children[i]);
    }
    printf ("Type: %d\nValue: %d\n", node->type, node->val.num);
    return 0;
}
// -------------------------------------------------------------------------------------------------------
