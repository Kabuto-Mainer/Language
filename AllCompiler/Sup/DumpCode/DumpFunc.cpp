#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "DumpFunc.h"
#include "TypesOfType.h"
#include "SupportFunc.h"
#include "DumpConfig.h"
#include "TokenFunc.h"
#include "AllLexeme.h"
#include "DumpLexeme.h"

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

// -------------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи информации о типе для дампа в строку
 * \param [in] type Указатель на тип
 * \param [in] extra_inf Указатель на строку
 */
void printTypeInfo (TypeEntry_t* type,
                    char* extra_inf)
{
    if (type == NULL)
        return;

    assert (extra_inf);

    if (type->kind == TYPE_STANDARD)
    {
        sprintf (extra_inf,
            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">STANDARD TYPE</TD>"
            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%p (%s)</TD></TR>\n",
            type, type->name->string);
    }
    else if (type->kind == TYPE_STRUCT)
    {
        sprintf (extra_inf,
            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">STRUCT</TD>"
            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%p (%s)</TD></TR>\n",
            type, type->name->string);
    }
    else if (type->kind == TYPE_ARRAY)
    {
        sprintf (extra_inf,
            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">ARRAY</TD>"
            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%p (%s)</TD></TR>\n"
            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">SIZE</TD>"
            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%d</TD></TR>\n",
            type, type->info.array_info.elem_type->name->string,
            type->info.array_info.amount);
    }
    else
    {
        sprintf (extra_inf,
            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">POINTER</TD>"
            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%p (%s)</TD></TR>\n",
            type, type->info.ptr_type->name->string);
    }
    return;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
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
    char type[30] = "";
    char color[10] = "";
    char label[100] = "";
    char extra_info[1000] = "";

    switch (node->type_node)
    {
        case NODE_KEY:
            strcpy (color, "#0f32e0ff");
            strcpy (type, "KEYWORD");
            sprintf (label, "KW_%d (%s))", node->val.key,
                DUMP_KEYWORDS[node->val.key]);
            break;

        case NODE_ID_RAW:
            strcpy (color, "#1bc3a7ff");
            strcpy (type, "ID_RAW");
            if (node->val.id_raw.name != NULL)
                sprintf (label, "\"%s\"", node->val.id_raw.name->string);
            else
                sprintf (label, "NULL");
            break;

        case NODE_NUM:
            strcpy (color, "#de3434ff");
            strcpy (type, "NUMBER");
            sprintf (label, "%d", node->val.num);
            break;

        case NODE_OPER:
            strcpy (color, "#e8802bff");
            strcpy (type, "OPERATOR");
            sprintf (label, "OP_%d (%s)", node->val.oper.code,
                DUMP_OPER[node->val.oper.code]);

            if (node->val.oper.type != NULL)
            {
                printTypeInfo (node->val.oper.type, extra_info);
            }
            break;

        case NODE_PUNCT:
            strcpy (color, "#ddf331ff");
            strcpy (type, "PUNCT");
            sprintf (label, "PUNCT_%d", node->val.punct);
            break;

        case NODE_ID_TREE:
            strcpy (color, "#4fdc36ff");
            strcpy (type, "ID_TREE");
            if (node->val.id_tree.name != NULL)
            {
                sprintf (label, "\"%s\"", node->val.id_tree.name->string);

                if (node->val.id_tree.type.srt.name != NULL)
                {
                    sprintf (extra_info,
                            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">TYPE</TD>"
                            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%s</TD></TR>\n"
                            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">PTR_LVL</TD>"
                            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%d</TD></TR>\n"
                            "<TR><TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">AMOUNT</TD>"
                            "<TD BGCOLOR=\"#386de7ff\" COLSPAN=\"1\">%d</TD></TR>\n",
                            node->val.id_tree.type.srt.name->string,
                            node->val.id_tree.type.srt.ptr_lvl,
                            node->val.id_tree.type.srt.array_size);
                }
            }
            else
                sprintf (label, "NULL");
            break;

        case NODE_ID_RESOLVED:
            strcpy (color, "#2eb82eff");
            strcpy (type, "ID_RESOLVED");
            sprintf (label, "%s", node->val.id_res.symbol->name->string);
            break;

        case NODE_CALL:
            strcpy (color, "#ff6ec7ff");
            strcpy (type, "CALL");
            sprintf (label, "call");
            break;

        case NODE_INDEX:
            strcpy (color, "#ffa500ff");
            strcpy (type, "INDEX");
            sprintf (label, "[ ]");
            break;

        case NODE_FIELD:
            strcpy (color, "#00ced1ff");
            strcpy (type, "FIELD");
            sprintf (label, ".");
            break;

        case NODE_ASSIGN:
            strcpy (color, "#ff4500ff");
            strcpy (type, "ASSIGN");
            sprintf (label, "=");
            break;

        case NODE_IF:
            strcpy (color, "#9370dbff");
            strcpy (type, "IF");
            sprintf (label, "if");
            break;

        case NODE_ELSE_IF:
            strcpy (color, "#8a2be2ff");
            strcpy (type, "ELSE_IF");
            sprintf (label, "elif");
            break;

        case NODE_ELSE:
            strcpy (color, "#7b68eeff");
            strcpy (type, "ELSE");
            sprintf (label, "else");
            break;

        case NODE_WHILE:
            strcpy (color, "#20b2aaff");
            strcpy (type, "WHILE");
            sprintf (label, "while");
            break;

        case NODE_RETURN:
            strcpy (color, "#dc143cff");
            strcpy (type, "RETURN");
            sprintf (label, "return");
            break;

        case NODE_BREAK:
            strcpy (color, "#ff6347ff");
            strcpy (type, "BREAK");
            sprintf (label, "break");
            break;

        case NODE_CONTINUE:
            strcpy (color, "#ff7f50ff");
            strcpy (type, "CONTINUE");
            sprintf (label, "continue");
            break;

        case NODE_VAR_DECL:
            strcpy (color, "#c1f453ff");
            strcpy (type, "VAR_DECL");
            sprintf (label, "variable");
            break;

        case NODE_FUNC_DECL:
            strcpy (color, "#32cd32ff");
            strcpy (type, "FUNC_DECL");
            sprintf (label, "function");
            break;

        case NODE_STRUCT_DECL:
            strcpy (color, "#98fb98ff");
            strcpy (type, "STRUCT_DECL");
            sprintf (label, "struct");
            break;

        case NODE_BLOCK:
            strcpy (color, "#778899ff");
            strcpy (type, "BLOCK");
            sprintf (label, "{ }");
            break;

        case NODE_PROGRAM:
            strcpy (color, "#4169e1ff");
            strcpy (type, "PROGRAM");
            sprintf (label, "PROGRAM");
            break;

        case NODE_COND_LIST:
            strcpy (color, "#6a5acdff");
            strcpy (type, "COND_LIST");
            sprintf (label, "if-elif-else");
            break;

        default:
            strcpy (color, "#d119ffff");
            strcpy (type, "UNKNOWN");
            sprintf (label, "???");
    }

    fprintf (stream, "block_%p [shape=%s, label=<\n"
            "<TABLE CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
            "<TR><TD PORT=\"root\" BGCOLOR=\"%s\" COLSPAN=\"2\"><B>%p</B></TD></TR>\n",
            node, shape, color, node);

    fprintf (stream, "<TR><TD BGCOLOR=\"#b209ccff\" COLSPAN=\"2\">PARENT=%p</TD></TR>\n",
            node->parent);

    fprintf (stream, "<TR><TD BGCOLOR=\"#6bd934ff\" COLSPAN=\"2\">%s</TD></TR>\n",
            type);

    fprintf (stream, "<TR><TD BGCOLOR=\"#46f2f5ff\" COLSPAN=\"2\">%s</TD></TR>\n",
            label);

    if (extra_info[0] != '\0')
        fprintf (stream, "%s", extra_info);

    fprintf (stream, "<TR><TD PORT=\"child\" BGCOLOR=\"#ff7301ff\" COLSPAN=\"2\">"
            "CHILDREN [%d]</TD></TR>\n",
            node->amount_children);

    fprintf (stream, "</TABLE> >];\n\n");

    return 0;
}
// ------------------------------------------------------------------------------------------------------

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

// // -------------------------------------------------------------------------------------------------------
// int dumpNode (Node_t* node)
// {
//     assert (node);
//
//     printf ("[%p]\nAM: %d\n", node, node->amount_children);
//     for (int i = 0; i < node->amount_children; i++)
//     {
//         printf ("    %p\n", node->children[i]);
//     }
//     printf ("Type: %d\nValue: %d\n", node->type, node->val.num);
//     return 0;
// }
// // -------------------------------------------------------------------------------------------------------
