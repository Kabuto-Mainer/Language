
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "SupportFunc.h"
#include "ParserFunc.h"
#include "TypesOfType.h"
#include "TokenFunc.h"
#include "DumpFunc.h"
#include "NodeFunc.h"

// ---------------------------------------------------------------------------------------------------
#define SYNTAX_ERROR(__context__, __number_error__, __ret_val__)    \
    do {                                                            \
        parserError (__number_error__,__context__);                 \
        EXIT_FUNC ("Syntax Error",__ret_val__);                     \
    } while (0)
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция создания связей в массиве узлов
 @param [in] start_node Начальный узел массива
 @param [in] amount Количество элементов в массива узлов
 @param [in] global_node Корень всего дерева
 @param [in] name_file Файл с кодом
*/
Status_t parserGlobal (Node_t* start_node, size_t amount,
                       Node_t* global_node, const char* name_file)
{
    assert (start_node);
    assert (global_node);
    assert (name_file);

    ParserContextInf_t inf =
    {
        .node = start_node,
        .line = 1,
        .error = PE_NOT_ERROR,
        .name_file = name_file,
        .cur_index = 0,
        .capacity = amount,
    };

    while (inf.cur_index < inf.capacity)
    {
        skipVoid (&inf);
        if (parserDeclarationFunc (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (parserDeclarationVar (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (inf.node->type == NODE_TYPE_INDENT)
        {
            if (isLeftTang (getNextNotEndNode (&inf)))
            {
                if (parserCallFunc (&inf, global_node) != PARSER_THIS_OK)
                    SYNTAX_ERROR (&inf, PE_INDENT_NO_END);
            }
            else
            {
                if (parserAssign (&inf, global_node) != PARSER_THIS_OK)
                    SYNTAX_ERROR (&inf, PE_INDENT_NO_END);
            }
            continue;
        }
        if (parserCondOper (&inf, global_node) == PARSER_THIS_OK)
            continue;
        if (parserReturn (&inf, global_node) == PARSER_THIS_OK)
            continue;
        if (parserInOut (&inf, global_node) == PARSER_THIS_OK)
            continue;
        if (parserBreak (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (inf.cur_index == inf.capacity - 1)
            break;
        SYNTAX_ERROR (&inf, PE_UNKNOWN);
    }
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------




// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания блока коды
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserUnion (ParserContextInf_t* inf,
                      Node_t* node)
{
    assert (inf);
    assert (node);

    if (!isLeftRound (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_LEFT_ROUND);

    Node_t* block = inf->node;
    block->type = NODE_TYPE_BLOCK;
    block->val.name = NULL;
    addNode (node, block);
    nextNode (inf);

    while (true)
    {
        skipVoid (inf);
        if (parserDeclarationVar (inf, block) == PARSER_THIS_OK)
            continue;
        if (parserReturn (inf, block) == PARSER_THIS_OK)
            continue;
        if (inf->node->type == NODE_TYPE_INDENT)
        {
            if (isLeftTang (getNextNotEndNode (inf)))
            {
                if (parserCallFunc (inf, block) != PARSER_THIS_OK)
                    SYNTAX_ERROR (inf, PE_CALL_FUNC);
            }
            else
            {
                if (parserAssign (inf, block) != PARSER_THIS_OK)
                    SYNTAX_ERROR (inf, PE_ASSIGN);
            }
            continue;
        }
        if (parserCondOper (inf, block) == PARSER_THIS_OK)
            continue;
        if (parserReturn (inf, block) == PARSER_THIS_OK)
            continue;
        if (parserInOut (inf, block) == PARSER_THIS_OK)
            continue;
        if (parserBreak (inf, block) == PARSER_THIS_OK)
            continue;

        break;
    }
    if (!isRightRound (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);
    nextNode (inf);
    skipVoid (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания операций in и out
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] parent Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserInOut (ParserContextInf_t* inf,
                      Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
        (inf->node->val.key != KEY_IN &&
         inf->node->val.key != KEY_OUT))
        return PARSER_NOT_THIS;

    Node_t* node_key = inf->node;
    nextNode (inf);
    skipVoid (inf);

    if (!isLeftTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_LEFT_TANG);

    nextNode (inf);
    skipVoid (inf);

    if (node_key->val.key == KEY_IN)
    {
        if (inf->node->type != NODE_TYPE_INDENT)
            SYNTAX_ERROR (inf, PE_NOT_INDENT);
        addNode (node_key, inf->node);
        inf->node->type = NODE_TYPE_VAR;
        nextNode (inf);
        skipVoid (inf);
    }
    else
    {
        if (parserExpresion (inf, node_key) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
    }
    addNode (parent, node_key);
    if (!isRightTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_TANG);

    nextNode (inf);
    skipCharNext (inf);

    if (!isEndChar (inf->node) && !isRightRound (inf->node))
        SYNTAX_ERROR (inf, PE_NO_END_CHAR);

    skipVoid (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания оператора с условием (if elif else while)
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserCondOper (ParserContextInf_t* inf,
                         Node_t* node)
{
    assert (inf);
    assert (node);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
       (inf->node->val.key != KEY_IF &&
        inf->node->val.key != KEY_ELSE_IF &&
        inf->node->val.key != KEY_ELSE &&
        inf->node->val.key != KEY_WHILE))
        return PARSER_NOT_THIS;

    Node_t* cond_node = inf->node;
    addNode (node, cond_node);
    nextNode (inf);
    skipVoid (inf);

    if (cond_node->val.key != KEY_ELSE)
    {
        if (!isLeftTang (inf->node))
            SYNTAX_ERROR (inf, PE_NOT_LEFT_TANG);

        nextNode (inf);
        skipVoid (inf);

        if (parserExpresion (inf, cond_node) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_CONDITION);

        if (!isRightTang (inf->node))
            SYNTAX_ERROR (inf, PE_NOT_RIGHT_TANG);

        nextNode (inf);
        skipVoid (inf);
    }

    if (parserUnion (inf, cond_node) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_UNION);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания приравнивания
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserAssign (ParserContextInf_t* inf,
                       Node_t* node)
{
    assert (inf);
    assert (node);

    Node_t* indet_node = inf->node;
    indet_node->type = NODE_TYPE_VAR;
    nextNode (inf);
    skipVoid (inf);

    if (inf->node->type == NODE_TYPE_KEY_WORD &&
        inf->node->val.key == KEY_ASSIGN)
    {
        Node_t* assign = inf->node;
        addNode (node, assign);
        addNode (assign, indet_node);

        nextNode (inf);
        skipCharNext (inf);
        if (parserExpresion (inf, assign) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

        if (!isEndChar (inf->node) && !isRightRound (inf->node))
            SYNTAX_ERROR (inf, PE_NO_END_CHAR);

        return PARSER_THIS_OK;
    }
    else if (isEndChar (inf->node) || isRightRound (inf->node))
    {
        skipVoid (inf);
        return PARSER_THIS_OK;
    }
    else
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция считывания ключевого слова return
 * \param [in] inf Указатель на структуру с текущим положением
 * \param [in] parent Указатель на родителя поддерева
 * \return  PARSER_THIS_OK - если все нормально и это нужное место
            PARSER_NOT_THIS - если это не то место
            PARSER_ERROR - если произошла ошибка
*/
Status_t parserReturn (ParserContextInf_t* inf,
                       Node_t* node)
{
    assert (inf);
    assert (node);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
        inf->node->val.key != KEY_RETURN)
        return PARSER_NOT_THIS;

    Node_t* ret_node = inf->node;
    addNode (node, ret_node);

    nextNode (inf);
    skipCharNext (inf);

    if (parserExpresion (inf, ret_node) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

    skipVoid (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция считывания цикла while
 * \param [in] inf Указатель на структуру с текущим положением
 * \param [in] parent Указатель на родителя поддерева
 * \return  PARSER_THIS_OK - если все нормально и это нужное место
            PARSER_NOT_THIS - если это не то место
            PARSER_ERROR - если произошла ошибка
*/
Status_t parserWhile (ParserContextInf_t* inf,
                      Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
        inf->node->val.key != KEY_WHILE)
        return PARSER_NOT_THIS;

    Node_t* node_while = inf->node;
    addNode (parent, node_while);
    nextNode (inf);
    skipVoid (inf);

    if (!isLeftTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_LEFT_TANG);

    nextNode (inf);
    skipVoid (inf);

    if (parserExpresion (inf, node_while) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_CONDITION);

    if (!isRightTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_TANG);

    nextNode (inf);
    skipVoid (inf);

    if (parserUnion (inf, node_while) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_UNION);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания break
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t P_ParseBreak (ParserInf_t* inf,
                       Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_BREAK)
        return PARSER_NOT_THIS;

    Node_t* node_break = inf->ctx->node;
    addNode (parent, node_break);
    node_break->type_node = NODE_BREAK;

    nextNode (inf->ctx);

    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

Status_t P_ParseContinue (ParserInf_t* inf,
                          Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_CONTINUE)
        return PARSER_NOT_THIS;

    Node_t* node_break = inf->ctx->node;
    addNode (parent, node_break);
    node_break->type_node = NODE_CONTINUE;

    nextNode (inf->ctx);

    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

Status_t P_ParseReturn (ParserInf_t* inf,
                        Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_RETURN)
        return PARSER_NOT_THIS;

    Node_t* node_break = inf->ctx->node;
    addNode (parent, node_break);
    node_break->type_node = NODE_RETURN;

    nextNode (inf->ctx);

    if (!isEndChar (inf->ctx->node) && !isRightRound (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
/**
 \brief Функция считывания объявления функции
 \param [in] inf Указатель на структуру парсера
 \param [in] node Указатель на родителя поддерева
 \return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t P_ParseDeclarationFunc (ParserInf_t* inf,
                                 Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* decl_node = inf->ctx->node;
    if (decl_node->type_node != NODE_KEY &&
        decl_node->val.key != KW_FUNCTION)
        return PARSER_NOT_THIS;

    decl_node->type_node = NODE_FUNC_DECL;
    nextNode (inf->ctx);

    Node_t* func_id = inf->ctx->node;
    if (func_id->type_node != NODE_ID_RAW)
        SYNTAX_ERROR (inf->ctx, PE_NOT_IDENT, PARSER_SYNTAX_ERROR);

    func_id->type_node = NODE_ID_TREE;
    func_id->val.id_tree.name = func_id->val.id_raw.name;

    addNode (parent, decl_node);
    addNode (decl_node, func_id);
    nextNode (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    while (true)
    {
        Node_t* param = inf->ctx;
        if (param->type_node != NODE_ID_RAW)
            SYNTAX_ERROR (inf->ctx, PE_NOT_PARAM, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        if (inf->ctx->node->type_node != NODE_PUNCT &&
            inf->ctx->node->val.punct != PUNCT_COLON)
            SYNTAX_ERROR (inf->ctx, PE_NOT_COLON, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        TypeEntry_t* type_param = P_ParseType (inf);
        if (type_param == NULL)
            SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

        param->type_node = NODE_ID_TREE;
        param->val.id_tree.name = param->val.id_raw.name;
        param->val.id_tree.base_type = type_param;
        addNode (decl_node, param);

        if (isComma (inf->ctx->node))
        {
            nextNode (inf->ctx);
            continue;
        }
    }
    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (inf->ctx->node->type_node != NODE_PUNCT ||
        inf->ctx->node->val.punct != PUNCT_LPAREN)
        return PARSER_THIS_OK;

    if (P_ParseUnion (inf, decl_node) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания объявления переменной
 @param [in] inf Указатель на структуру парсера
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t P_ParseDeclarationVar (ParserInf_t* inf,
                                Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_VARIABLE)
        return PARSER_NOT_THIS;

    Node_t* define_node = inf->ctx->node;
    define_node->type_node = NODE_VAR_DECL;

    addNode (parent, define_node);
    nextNode (inf->ctx);
    // skipVoid (inf->ctx);

    TypeEntry_t* type_var = P_ParseType (inf);
    if (type_var == NULL)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    while (true)
    {
        Node_t* var_id = inf->ctx->node;
        if (var_id->type_node != NODE_ID_RAW)
            SYNTAX_ERROR (inf->ctx, PE_NOT_IDENT, PARSER_SYNTAX_ERROR);

        var_id->type_node = NODE_ID_TREE;
        var_id->val.id_tree.name = var_id->val.id_raw.name;
        var_id->val.id_tree.base_type = type_var;

        nextNode (inf->ctx);
        // skipVoid (inf->ctx);

        Node_t* next_node = inf->ctx->node;
        if (next_node->type_node == NODE_OPER &&
            next_node->val.oper.code == OPER_ASSIGN)
        {
            addNode (next_node, var_id);
            addNode (define_node, next_node);

            nextNode (inf->ctx);
            if (P_ParseExpression (inf, next_node) != PARSER_THIS_OK)
                SYNTAX_ERROR (inf->ctx, P_ERROR, PARSER_SYNTAX_ERROR);
        }
        else
        {
            addNode (define_node, var_id);
        }

        if (isComma (inf->ctx->node))
        {
            nextNode (inf->ctx);
            continue;
        }
        break;
    }

    if (!isEndChar (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NO_END_CHAR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция чтения типа
 * \param [in] inf Указатель на структуру парсера
 * \return Указатель на тип в таблице типов, и NULL в случае ошибки
 */
TypeEntry_t* P_ParseType (ParserInf_t* inf)
{
    assert (inf);

    int ptr_lvl = 0;
    while (inf->ctx->node->type_node == NODE_OPER &&
           inf->ctx->node->val.oper.code == OPER_ADDR)
    {
        ptr_lvl++;
        nextNode (inf->ctx);
    }

    if (inf->ctx->node->type_node != NODE_ID_RAW)
        SYNTAX_ERROR (inf->ctx, PE_UNKNOWN_TYPE, NULL);

    StringEntry_t* name_type = inf->ctx->node->val.id_raw.name;
    TypeEntry_t* type = TT_Find (inf->type_table, name_type);
    if (type == NULL)
        SYNTAX_ERROR (inf->ctx, PE_UNKNOWN_TYPE, NULL);

    for (int i = 0; i < ptr_lvl; i++)
        type = TT_GetPointerType (inf->type_table, type);

    nextNode (inf->ctx);
    if (isLeftQuad (inf->ctx->node))
    {
        nextNode (inf->ctx);
        if (inf->ctx->node->type_node != NODE_NUM)
            SYNTAX_ERROR (inf->ctx, PE_UNKNOWN_ARRAY_SIZE, NULL);

        int amount = inf->ctx->node->val.num;
        if (amount <= 0)
            SYNTAX_ERROR (inf->ctx, PE_ARRAY_BAD_SIZE, NULL);


        nextNode (inf->ctx);
        if (!isRightQuad (inf->ctx->node))
            SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_QUAD, NULL);

        type = TT_GetArrayType (inf->type_table, type, amount);
    }
    return type;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseOrExpression (ParserInf_t* inf,
                              Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode ();
    if (P_ParseAndExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           inf->ctx->node->val.oper.code == OPER_OR)
    {
        Node_t* logic_or = inf->ctx->node;
        addChildren (logic_or, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseAndExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
        }

        addChildren (logic_or, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode();
        addNode (buffer_node, logic_or);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseAndExpression (ParserInf_t* inf,
                               Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode ();
    if (P_ParseEqualExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           inf->ctx->node->val.oper.code == OPER_AND)
    {
        Node_t* logic_and = inf->ctx->node;
        addChildren (logic_and, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseEqualExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
        }

        addChildren (logic_and, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, logic_and);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseEqualExpression (ParserInf_t* inf,
                                 Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode ();
    if (P_ParseCompareExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_E ||
            inf->ctx->node->val.oper.code == OPER_NE))
    {
        Node_t* eq_node = inf->ctx->node;
        addChildren (eq_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseCompareExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
        }

        addChildren (eq_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, eq_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseCompareExpression (ParserInf_t* inf,
                                   Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode();
    if (P_ParseAddSub (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_L ||
            inf->ctx->node->val.oper.code == OPER_B ||
            inf->ctx->node->val.oper.code == OPER_LE ||
            inf->ctx->node->val.oper.code == OPER_BE))
    {
        Node_t* cmp_node = inf->ctx->node;
        addChildren (cmp_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseAddSub (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
        }

        addChildren (cmp_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, cmp_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseUnaryExpression (ParserInf_t* inf,
                                 Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node == NODE_OPER &&
        (inf->ctx->node->val.oper.code == OPER_NOT ||
         inf->ctx->node->val.oper.code == OPER_ADDR ||
         inf->ctx->node->val.oper.code == OPER_DEREF))
    {
        Node_t* unary_node = inf->ctx->node;
        nextNode (inf->ctx);
        addNode (parent, unary_node);

        if (P_ParseUnaryExpression (inf, unary_node) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);

        return PARSER_THIS_OK;
    }
    return P_ParsePrimaryExpression (inf, parent);
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseAddSub (ParserInf_t* inf,
                        Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode();
    if (P_ParseMulDiv (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_ADD ||
            inf->ctx->node->val.oper.code == OPER_SUB))
    {
        Node_t* oper_node = inf->ctx->node;
        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseMulDiv (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
        }

        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, oper_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseMulDiv (ParserInf_t* inf,
                        Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* buffer_node = newNode();
    if (P_ParseUnaryExpression (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
    }

    while (inf->ctx->node->type_node == NODE_OPER &&
           (inf->ctx->node->val.oper.code == OPER_DIV ||
            inf->ctx->node->val.oper.code == OPER_MUL))
    {
        Node_t* oper_node = inf->ctx->node;
        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);
        nextNode (inf->ctx);

        buffer_node = newNode ();
        if (P_ParseUnaryExpression (inf, buffer_node) != PARSER_THIS_OK)
        {
            deleteOneNode (buffer_node);
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);
        }

        addChildren (oper_node, buffer_node);
        deleteOneNode (buffer_node);

        buffer_node = newNode ();
        addNode (buffer_node, oper_node);
    }

    addChildren (parent, buffer_node);
    deleteOneNode (buffer_node);

    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParsePrimaryExpression (ParserInf_t* inf,
                                   Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (isLeftRound (inf->ctx->node))
    {
        nextNode (inf->ctx);
        if (P_ParseOrExpression (inf, parent) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);

        // skipVoid (inf);
        if (!isRightRound (inf->ctx->node))
            SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        return PARSER_THIS_OK;
    }
    if (inf->ctx->node->type_node == NODE_KEY &&
        inf->ctx->node->val.key == KW_CALL)
    {
        return P_ParseCallFunction (inf, parent);
    }
    else if (inf->ctx->node->type_node == NODE_NUM)
    {
        addNode (parent, inf->ctx->node);
        nextNode (inf->ctx);
        return PARSER_THIS_OK;
    }
    else if (inf->ctx->node->type_node == NODE_ID_RAW)
    {
        addNode (parent, inf->ctx->node);
        inf->ctx->node->val.id_tree.name = inf->ctx->node->val.id_raw.name;
        inf->ctx->node->type_node = NODE_ID_TREE;
        nextNode (inf->ctx);
        return PARSER_THIS_OK;
    }

    return PARSER_SYNTAX_ERROR;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseCallFunction (ParserInf_t* inf,
                              Node_t* parent)
{
    assert (inf);
    assert (parent);

    Node_t* node_call = inf->ctx->node;
    node_call->type_node = NODE_CALL;
    addNode (parent, node_call);
    nextNode (inf->ctx);

    if (inf->ctx->node->type_node != NODE_ID_RAW)
        SYNTAX_ERROR (inf->ctx, PE_NOT_IDENT, PARSER_SYNTAX_ERROR);

    Node_t* node_func = inf->ctx->node;
    node_func->type_node = NODE_ID_TREE;
    node_func->val.id_tree.name = node_func->val.id_raw.name;
    addNode (node_call, node_func);
    nextNode (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    while (true)
    {
        if (P_ParseOrExpression (inf, node_call) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);

        if (isComma (inf->ctx->node))
        {
            nextNode (inf->ctx);
            skipVoid (inf->ctx);
            continue;
        }
        break;
    }

    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
Status_t P_ParseConditional (ParserInf_t* inf,
                             Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_IF)
        return PARSER_NOT_THIS;

    Node_t* node_if = inf->ctx->node;
    node_if->type_node = NODE_IF;
    nextNode (inf->ctx);

    if (!isLeftTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

    /* Т.к. создавать узлы нежелательно, поэтому используем все, что есть */
    Node_t* node_list = inf->ctx->node;
    node_list->type_node = NODE_COND_LIST;

    addNode (parent, node_list);
    addNode (node_list, node_if);
    nextNode (inf->ctx);

    if (P_ParseOrExpression (inf, node_if) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);

    if (!isRightTang (inf->ctx->node))
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    nextNode (inf->ctx);
    if (P_ParserUnion (inf, node_if) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

    skipVoid (inf->ctx);
    while (inf->ctx->node->type_node == NODE_KEY &&
           inf->ctx->node->val.key == KW_ELIF)
    {
        Node_t* node_elif = inf->ctx->node;
        node_elif->type_node = NODE_ELSE_IF;

        nextNode (inf->ctx);
        if (!isLeftTang (inf->ctx->node))
            SYNTAX_ERROR (inf->ctx, PE_NOT_LEFT_TANG, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        if (P_ParseOrExpression (inf, node_elif) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_NOT_EXPRESION, PARSER_SYNTAX_ERROR);

        if (!isRightTang (inf->ctx->node))
            SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

        nextNode (inf->ctx);
        skipVoid (inf->ctx);
        if (P_ParserUnion (inf, node_elif) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf->ctx, PE_ERROR, PARSER_SYNTAX_ERROR);

        addNode (node_list, node_elif);
        skipVoid (inf->ctx);
    }
    if (inf->ctx->node->type_node != NODE_KEY ||
        inf->ctx->node->val.key != KW_ELSE)
        return PARSER_THIS_OK;

    Node_t* node_else = inf->ctx->node;
    node_else->type_node = NODE_ELSE;
    nextNode (inf->ctx);
    skipVoid (inf->ctx);

    if (P_ParserUnion (inf, node_else) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf->ctx, PE_NOT_RIGHT_TANG, PARSER_SYNTAX_ERROR);

    addNode (node_list, node_else);
    return PARSER_THIS_OK;
}

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция проверки узла на соответствие с типом пунктуации и значением
 * \param [in] node Узел, который проверяют
 * \param [in] val ЗначениеЮ которое должно быть у узла
 * \return 1 - если совпадает, иначе 0
 */
int checkPunctNode (Node_t* node, int val)
{
    assert (node);

    if (node->type_node == NODE_PUNCT &&
        node->val.punct == val)
        return 1;

    return 0;
}

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция смены текущего узла в контексте
 @param [in] inf Указатель на структуру контекста
*/
int nextNode (ParserContextInf_t* inf)
{
    assert (inf);

    if (inf->cur_index == inf->capacity - 1)
        return 1;
    // tokenOneDump (inf->node, "Bla Bla Bla");

    if (inf->node->type_node == NODE_PUNCT &&
        inf->node->val.punct == PUNCT_ENDSTRING)
        inf->line++;

    inf->node = inf->node + 1;
    inf->cur_index++;
    return 0;
}

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция пропуска всех разделяющий символов (\n и \)
 * \param [in] inf Указатель на структуру контекста
*/
int skipVoid (ParserContextInf_t* inf)
{
    assert (inf);

    while (inf->node->type_node == NODE_PUNCT &&
           inf->node->val.punct == PUNCT_ENDSTRING)
    {
        if (inf->cur_index == inf->capacity - 1)
            return 1;

        inf->node = inf->node + 1;
        inf->cur_index++;
        inf->line++;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция выдачи следующего неконечного контекстного токена
 @param [in] inf Указатель на структуру контекста
 @return Узел
*/
Node_t* getNextNotEndNode (ParserContextInf_t* inf)
{
    assert (inf);
    Node_t* node = inf->node + 1;

    while (inf->node->type_node == NODE_PUNCT &&
           inf->node->val.punct == PUNCT_ENDSTRING)
        node = node + 1;

    return node;
}

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция вывода ошибки по её номеру
 @param [in] error Номер ошибки
 @param [in] inf Указатель на структуру контекста
*/
int parserError (ParserError_t error,
                 ParserContextInf_t* inf)
{
    assert (inf);

    if (inf->error != PE_NOT_ERROR)
        return 0;

    inf->error = error;
    printf ("Syntax Error in %s:%d: ",
    inf->name_file, inf->line);

    switch (error)
    {
        case PE_UNKNOWN:
            printf ("Unknown syntax error");
            break;

        case PE_INDENT_NO_END:
            printf ("Identifier not properly terminated (missing \'\\n\')");
            break;

        case PE_ASSIGN:
            printf ("Invalid assignment statement");
            break;

        case PE_CALL_FUNC:
            printf ("Invalid function call syntax");
            break;

        case PE_NOT_CONDITION:
            printf ("Missing or invalid condition in control statement");
            break;

        case PE_NO_END_CHAR:
            printf ("Missing statement terminator (\'\\n\')");
            break;

        case PE_NOT_RIGHT_ROUND:
            printf ("Missing closing parenthesis ')'");
            break;

        case PE_NOT_LEFT_ROUND:
            printf ("Missing opening parenthesis '('");
            break;

        case PE_NOT_LEFT_TANG:
            printf ("Missing opening angle bracket '<'");
            break;

        case PE_NOT_RIGHT_TANG:
            printf ("Missing closing angle bracket '>'");
            break;

        case PE_NOT_UNION:
            printf ("Missing operator between expressions");
            break;

        case PE_NOT_EXPRESION:
            printf ("Expected expression but found something else");
            break;

        case PE_NOT_INDENT:
            printf ("Expected identifier but found something else");
            break;

        case PE_NOT_RETURN_VALUE:
            printf ("Missing return value of function");
            break;

        case PE_NOT_TYPE:
            printf ("Missing type of variable");
            break;

        case PE_NOT_ERROR:
            break;

        default:
            printf ("Unhandled parser error (code: %d)", error);
    }

    printf("\n");
    return 0;
}
