#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "NodeType.h"
#include "Analisis.h"
#include "SupportFunc.h"


enum AnalysisStatus_t
{
    ANALYSIS_OK,
    ANALYSIS_ERROR
};

TypeEntry_t* getType (TypeTable_t* type_table,
                      TreeType_t type_tree)
{
    assert (type_table);
    assert (type_tree.srt.name);

    TypeEntry_t* type = TT_Find (type_table, type_tree.srt.name);
    if (type == NULL)
        return NULL;

    for (int i = 0; i < type_tree.srt.ptr_lvl; i++)
        type = TT_GetPointerType (type_table, type);

    if (type_tree.srt.array_size != 0)
        type = TT_GetArrayType (type_table, type, type_tree.srt.array_size);

    return type;
}

AnalysisStatus_t A_CollectDeclarationType (AnalysisContextInf_t* info,
                                           Node_t* root)
{
    assert (info);
    assert (root);

    for (int index_root = 0; index_root < root->amount_children; index_root++)
    {
        if (root->children[index_root]->type_node != NODE_STRUCT_DECL)
            continue;

        Node_t* node_struct = root->children[index_root];
        StringEntry_t* name_struct = node_struct->val.struct_def.name;

        if (TT_Find (info->table_type, name_struct) != NULL)
            ANALYS_ERROR (info, A_REDECL_TYPE, ANALYSIS_ERROR);

        TypeEntry_t* struct_type = TT_Add (info->table_type, name_struct,
                                           TYPE_STRUCT, 1);

        int size_struct = 0;
        for (int i = 0; i < node_struct->amount_children; i++)
        {
            Node_t* field = node_struct->children[i];
            TypeEntry_t* type_field = getType (info->table_type, field->val.id_tree.type);
            if (type_field == NULL)
                ANALYS_ERROR (info, A_UNKNOWN_TYPE, ANALYSIS_ERROR);

            TT_AddStructField (struct_type, field->val.id_tree.name, type_field);
        }
    }
    return ANALYSIS_OK;
}



/* Именно в этой функции создается таблица имен параметров для функции.
    Эта таблица хранится в узле имени функции*/
AnalysisStatus_t A_CollectDeclarationFunc (AnalysisContextInf_t* info,
                                           Node_t* root)
{
    assert (info);
    assert (root);

    for (int index_root = 0; index_root < root->amount_children; index_root++)
    {
        if (root->children[index_root]->type_node != NODE_FUNC_DECL)
            continue;

        Node_t* func_decl = root->children[index_root];
        Node_t* node_func_name = func_decl->children[0];
        StringEntry_t* name_func = node_func_name->val.id_tree.name;

        if (SymT_FindLocal (info->table_func, name_func) != NULL)
            ANALYS_ERROR (info, A_REDECL_FUNC, ANALYSIS_ERROR);

        TypeEntry_t* ret_type = getType (info->table_type, func_decl->children[0]->val.id_tree.type);
        if (ret_type == NULL)
            ANALYS_ERROR (info, A_UNKNOWN_TYPE, ANALYSIS_ERROR);

        SymbolEntry_t* function = SymT_AddFunction (info->table_func, name_func, ret_type, SymT_Create (NULL));

        int size_struct = 0;
        for (int i = 1; i < func_decl->amount_children; i++)
        {
            Node_t* param = func_decl->children[i];
            if (param->type_node != NODE_ID_TREE)
            {
                if (i != func_decl->amount_children - 1)
                    ANALYS_ERROR (info, A_SYSTEM_BAD_TREE, ANALYSIS_ERROR);\

                break;
            }

            TypeEntry_t* type_param = getType (info->table_type, param->val.id_tree.type.srt);
            if (type_param == NULL)
            {
                ANALYS_ERROR (info, A_UNKNOWN_TYPE, ANALYSIS_ERROR);
            }

            SymT_AddFunctionParameter (function, param->val.id_tree.name->string, type_param);
        }
    }
    return ANALYSIS_OK;
}





/* Эта функция должна вызываться после собрания объявления всех функций,
   т.к. использует таблицы имен, которые должны быть созданы в собрании всех функций */

/* В время написания появился такой вопрос. Стоит ли разбивать процесс анализа переменных на несколько частей, а именно:
    Изначально был вариант собрания объявления всех переменных с полной типизацией, но с такой структурой возникала проблема,
    ``Если мы используем переменную до её объявления, то это не будет ошибкой. Как вариант, это может быть особенностью языка,
    но по факту это будет нашей недоработкой, т.к. мы увеличиваем возможность неопределенного поведения,
    что нам разумеется не надо (по крайней мере в философии языка такого нет)``
    Поэтому обработку объявления переменной функции и её использование было решено объединить в один этап
*/

AnalysisStatus_t A_CollectVariable (AnalysisContextInf_t* info,
                                    Node_t* parent, SymbolTable_t* current_table)
{
    assert (info);
    assert (parent);
    assert (current_table);

    for (int i = 0; i < parent->amount_children; i++)
    {
        Node_t* cur = parent->children[i];

/* Необходимо для нормальной работы при использовании флага -Werror */
/* Нет смысла в переборе всех значение, а прописывание каждого - не нужный труд */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"

        switch (cur->type_node)
        {

        /* Одинаковая структура. Создаем блок и проходимся по всем детям */
        case NODE_COND_LIST:
        case NODE_PROGRAM:
        case NODE_BLOCK:
            SymbolTable_t* block_table = SymT_Create (current_table);
            for (int i = 0; i < parent->amount_children; i++)
            {
                if (A_CollectVariable (info, cur, block_table) != ANALYSIS_OK)
                    ANALYS_ERROR (info, A_ERROR, ANALYSIS_ERROR);
            }
            break;

        /* Аналогично предыдущему блоку */
        case NODE_GLOBAL_DECL:
        case NODE_VAR_DECL:
            if (A_CollectDeclarationVariable (info, cur, current_table) != ANALYSIS_OK)
                ANALYS_ERROR (info, A_ERROR, ANALYSIS_ERROR);

            break;

        case NODE_FUNC_DECL:
            if (cur->children[cur->amount_children - 1]->type_node != NODE_BLOCK)
                continue;
            if (A_CollectVariable (info, cur->children[cur->amount_children - 1],
                cur->children[0]->val.func.params) != ANALYSIS_OK)
                ANALYS_ERROR (info, A_ERROR, ANALYSIS_ERROR);
            break;

        /* Тут даже не надо создавать таблицу имен. Она создастся во время обработки блока */
        case NODE_IF:
        case NODE_ELSE_IF:
        case NODE_ELSE:
        case NODE_OPER:
        case NODE_WHILE:
        case NODE_RETURN:
        case NODE_ASSIGN:
            for (int i = 0; i < parent->amount_children; i++)
            {
                if (A_CollectVariable (info, cur, current_table) != ANALYSIS_OK)
                    ANALYS_ERROR (info, A_ERROR, ANALYSIS_ERROR);
            }
            break;

        case NODE_CALL:
            Node_t* node_func = cur->children[0];
            SymbolEntry_t* func = SymT_FindLocal (info->table_func, node_func->val.id_tree.name);
            if (func == NULL)
                ANALYS_ERROR (info, A_UNKNOWN_FUNC, ANALYSIS_ERROR);
            node_func->type_node = NODE_CALL;
            node_func->val.id_res.symbol = func;
            for (int i = 1; i < parent->amount_children; i++)
            {
                if (A_CollectVariable (info, cur, current_table) != ANALYSIS_OK)
                    ANALYS_ERROR (info, A_ERROR, ANALYSIS_ERROR);
            }
            break;

        /* Сюда могут придти только переменные, хотя у функций может быть сначала тот-же тип */
        case NODE_ID_TREE:
            SymbolEntry_t* variable = SymT_FindGlobal (current_table, cur->val.id_tree.name);
            if (variable == NULL)
                ANALYS_ERROR (info, A_UNKNOWN_VAR, ANALYSIS_ERROR);
            cur->type_node = NODE_ID_RESOLVED;
            cur->val.id_res.symbol = variable;
            break;

        /* Ничего не должны делать в данном случае */
        case NODE_NUM:
        case NODE_BREAK:
        case NODE_CONTINUE:
            continue;

        default:
            ANALYS_ERROR (info, A_SYSTEM, ANALYSIS_ERROR);
        }

#pragma GCC diagnostic pop
    }
    return ANALYSIS_OK;
}

AnalysisStatus_t A_CollectDeclarationVariable (AnalysisContextInf_t* info,
                 Node_t* node_decl, SymbolTable_t* current_table)
{
    assert (info);
    assert (node_decl);
    assert (current_table);

    Node_t* var = NULL;
    if (node_decl->children[0]->type_node == NODE_ASSIGN)
        var = node_decl->children[0]->children[0];
    else
        var = node_decl->children[0];

    if (SymT_FindLocal (current_table, var->val.id_tree.name) != NULL)
        ANALYSIS_ERROR (info, A_REDECL_VAR, ANALYSIS_ERROR);

    TypeEntry_t* type_var = getType (info->table_type, var->val.id_tree.type.srt);
    if (type_var == NULL)
        ANALYS_ERROR (info, A_UNKNOWN_TYPE, ANALYSIS_ERROR);

    SymbolEntry_t* var_sym = SymT_AddEntry (current_table, SK_VAR, var->val.id_tree.name, type_var);
    var->type_node == NODE_ID_RESOLVED;
    var->val.id_res.symbol = var_sym;
    return ANALYSIS_OK;
}



TypeEntry_t* A_FindTypeOperation (AnalysisContextInf_t* info,
                                  Node_t* parent)
{
    assert (info);
    assert (parent);

    switch (parent->type_node)
    {
        case NODE_NUM:

        case NODE_ID_TREE:

        case NODE_OPER:

        case NODE_CALL:

        case NODE_INDEX:

        case NODE_FIELD:

        case NODE_IF:
        case NODE_ELSE_IF:

        case NODE_ELSE:
        case NODE_WHILE:

        case NODE_ASSIGN:
    }
}



int AnalysisGlobal (AnalysisContextInf_t* info,
                    Node_t* root)
{
    assert (info);
    assert (root);

    root->val.block.prev = NULL;
    root->val.block.scope = SymT_Create (NULL);



}

void printError (AnalisysError_t error)
{
    assert (format);

    printf ("Syntax Error: ")
    switch (error)
    {
        case:
        default:
    }
    return;
}


void printError (AnalisysError_t error, const char* format, ...)
{
    assert (format);

    printf ("Syntax Error: ")
    switch (error)
    {
        case:
        default:
    }

    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);

    return;
}



#define UNKNOWN_TYPE(__name__)                                                  \
    do {                                                                        \
        Log_Error ("Syntax Error. Unknown type");                               \
        printf ("Syntax Error: Unknown type (%s)\n", __name__);                 \
    } while (0)

#define UNKNOWN_VAR(__name__)                                                   \
    do {                                                                        \
        Log_Error ("Syntax Error. Unknown var");                                \
        printf ("Syntax Error: Unknown var (%)\n", __name__);                   \
    } while (0)
