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
#define SYNTAX_ERROR(__context__, __number_error__)     \
    do {                                                \
        parserError (__number_error__, __context__);    \
        EXIT_FUNC ("Syntax Error", PARSER_SYNTAX_ERROR);\
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
Status_t parserGlobal (Node_t* start_node,
                       size_t amount,
                       Node_t* global_node,
                       const char* name_file)
{
    assert (start_node);
    assert (global_node);
    assert (name_file);

    ParserContextInf_t inf = {
        .node = start_node,
        .line = 1,
        .error = PE_NOT_ERROR,
        .name_file = name_file,
        .cur_index = 0,
        .capacity = amount,
    };

    while (inf.cur_index < inf.capacity)
    {
        // printf ("sdbfjsbdfhsbdjfhbsjdhb\n");
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
    block->value.name = NULL;
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
        (inf->node->value.key != KEY_IN &&
         inf->node->value.key != KEY_OUT))
        return PARSER_NOT_THIS;

    Node_t* node_key = inf->node;
    nextNode (inf);
    skipVoid (inf);

    if (!isLeftTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_LEFT_TANG);

    nextNode (inf);
    skipVoid (inf);

    if (node_key->value.key == KEY_IN)
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
       (inf->node->value.key != KEY_IF &&
        inf->node->value.key != KEY_ELSE_IF &&
        inf->node->value.key != KEY_ELSE &&
        inf->node->value.key != KEY_WHILE))
        return PARSER_NOT_THIS;

    Node_t* cond_node = inf->node;
    addNode (node, cond_node);
    nextNode (inf);
    skipVoid (inf);

    if (cond_node->value.key != KEY_ELSE)
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
        inf->node->value.key == KEY_ASSIGN)
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
        inf->node->value.key != KEY_RETURN)
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
        inf->node->value.key != KEY_WHILE)
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
Status_t parserBreak (ParserContextInf_t* inf,
                      Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
        inf->node->value.key != KEY_BREAK)
        return PARSER_NOT_THIS;

    addNode (parent, inf->node);
    nextNode (inf);

    if (!isEndChar (inf->node) && !isRightRound (inf->node))
        SYNTAX_ERROR (inf, PE_NO_END_CHAR);

    skipVoid (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания объявления функции
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserDeclarationFunc (ParserContextInf_t* inf,
                                Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
        inf->node->value.key != KEY_DEFINE_FUNC)
        return PARSER_NOT_THIS;

    // tokenOneDump (inf->node, "Enter to declaration func");
    Node_t* node_define = inf->node;
    addNode (parent, node_define);
    nextNode (inf);
    skipCharNext (inf);

    // tokenOneDump (inf->node, "Before checking arguments");
    if (inf->node->type != NODE_TYPE_INDENT ||
        !isLeftTang (getNextNotEndNode (inf)))
        SYNTAX_ERROR (inf, PE_NOT_INDENT);

    addNode (node_define, inf->node);
    inf->node->type = NODE_TYPE_FUNC;
    nextNode (inf);
    nextNode (inf);

    while (true)
    {
        skipVoid (inf);
        if (inf->node->type == NODE_TYPE_INDENT)
        {
            addNode (node_define, inf->node);
            inf->node->type = NODE_TYPE_VAR;
            nextNode (inf);
            continue;
        }
        if (isComma (inf->node))
        {
            nextNode (inf);
            continue;
        }
        break;
    }
    if (!isRightTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_TANG);
    nextNode (inf);

    if (!isRightRound (inf->node) && !isEndChar (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

    skipVoid (inf);
    if (isLeftRound (inf->node))
    {
        if (parserUnion (inf, node_define) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_UNION);
    }

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания объявления переменной
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserDeclarationVar (ParserContextInf_t* inf,
                               Node_t* node)
{
    assert (inf);
    assert (node);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
        inf->node->value.key != KEY_DEFINE_VAR)
        return PARSER_NOT_THIS;

    addNode (node, inf->node);
    Node_t* ext = inf->node;
    nextNode (inf);
    // skipCharEnd (inf);

    if (inf->node->type != NODE_TYPE_INDENT)
        SYNTAX_ERROR (inf, PE_NOT_INDENT);

    Node_t* indent = inf->node;
    indent->type = NODE_TYPE_VAR;
    nextNode (inf);
    // skipCharEnd (inf);
    if (inf->node->type == NODE_TYPE_KEY_WORD &&
        inf->node->value.key == KEY_ASSIGN)
    {
        addNode (ext, inf->node);
        addNode (inf->node, indent);
        nextNode (inf);
        skipCharNext (inf);
        if (parserExpresion (inf, inf->node - 1) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
    }
    else    { addNode (ext, indent); }

    if (!isEndChar (inf->node) && !isRightRound (inf->node))
        SYNTAX_ERROR (inf, PE_NO_END_CHAR);

    nextNode (inf);
    skipVoid (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания выражения AddSub [+ '<>=' AddSub]
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserExpresion (ParserContextInf_t* inf,
                          Node_t* node)
{
    assert (inf);
    assert (node);

    Node_t* buffer_node = newNode ();
    if (parserAddSub (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
    }

    if (inf->node->type != NODE_TYPE_OPER ||
       (inf->node->value.oper != OPER_COMP_BIG_EQUAL &&
        inf->node->value.oper != OPER_COMP_ONLY_BIG &&
        inf->node->value.oper != OPER_COMP_LIT_EQUAL &&
        inf->node->value.oper != OPER_COMP_ONLY_LIT &&
        inf->node->value.oper != OPER_COMP_EQUAL &&
        inf->node->value.oper != OPER_COMP_NOT_EQUAL))
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    skipCharNext (inf);

    if (parserAddSub (inf, inf->node - 1) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания суммирования MulDiv
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserAddSub (ParserContextInf_t* inf,
                       Node_t* node)
{
    assert (inf);
    assert (node);

    Node_t* buffer_node = newNode ();
    if (parserMulDiv (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
    }

    if (inf->node->type != NODE_TYPE_OPER ||
       (inf->node->value.oper != OPER_ADD &&
        inf->node->value.oper != OPER_SUB))
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    skipCharNext (inf);
    if (parserAddSub (inf, inf->node - 1) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания умножения Power
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserMulDiv (ParserContextInf_t* inf,
                       Node_t* node)
{
    assert (inf);
    assert (node);

    Node_t* buffer_node = newNode ();
    if (parserValue (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
    }

    if (inf->node->type != NODE_TYPE_OPER ||
       (inf->node->value.oper != OPER_MUL &&
        inf->node->value.oper != OPER_DIV))
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    skipCharNext (inf);

    if (parserMulDiv (inf, inf->node - 1) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания значения
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserValue (ParserContextInf_t* inf,
                      Node_t* node)
{
    assert (inf);
    assert (node);

    if (isLeftRound (inf->node))
    {
        nextNode (inf);
        skipVoid (inf);
        if (parserExpresion (inf, node) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

        skipVoid (inf);
        if (!isRightRound (inf->node))
            SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

        nextNode (inf);
        skipCharNext (inf);
        return PARSER_THIS_OK;
    }
    if (parserNumber (inf, node) == PARSER_THIS_OK)
        return PARSER_THIS_OK;
    if (parserIndent (inf, node) == PARSER_THIS_OK)
        return PARSER_THIS_OK;

    SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания идентификатора
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserIndent (ParserContextInf_t* inf,
                       Node_t* parent)
{
    assert (inf);
    assert (parent);

    if (inf->node->type != NODE_TYPE_INDENT)
        return PARSER_NOT_THIS;

    if (isLeftTang (getNextNotEndNode(inf)))
    {
        if (parserCallFunc (inf, parent) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_INDENT);
    }
    else
    {
        if (parserVar (inf, parent) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_INDENT);
    }
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция считывания вызова функции
 * \param [in] inf Указатель на структуру с текущим положением
 * \param [in] parent Указатель на родителя поддерева
 * \return  PARSER_THIS_OK - если все нормально и это нужное место
            PARSER_NOT_THIS - если это не то место
            PARSER_ERROR - если произошла ошибка
*/
Status_t parserCallFunc (ParserContextInf_t* inf,
                         Node_t* parent)
{
    assert (inf);
    assert (parent);

    // printf ("CALL FUNC\n");
    Node_t* node_func = inf->node;
    addNode (parent, node_func);
    node_func->type = NODE_TYPE_FUNC;
    nextNode (inf);
    nextNode (inf); // Skip '<'

    while (true)
    {
        skipVoid (inf);
        if (isRightTang (inf->node))
            break;

        if (parserExpresion (inf, node_func) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

        skipVoid (inf);
        if (isComma (inf->node))
        {
            nextNode (inf);
            continue;
        }
        break;
    }
    if (!isRightTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_TANG);
    nextNode (inf);
    skipCharNext (inf);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция считывания переменной
 * \param [in] inf Указатель на структуру с текущим положением
 * \param [in] parent Указатель на родителя поддерева
 * \return  PARSER_THIS_OK - если все нормально и это нужное место
            PARSER_NOT_THIS - если это не то место
            PARSER_ERROR - если произошла ошибка
*/
Status_t parserVar (ParserContextInf_t* inf,
                    Node_t* parent)
{
    assert (inf);
    assert (parent);

    addNode (parent, inf->node);
    inf->node->type = NODE_TYPE_VAR;
    nextNode (inf);
    skipCharNext (inf);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания числа
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserNumber (ParserContextInf_t* inf,
                       Node_t* parent)
{
    assert (inf);

    if (inf->node->type != NODE_TYPE_NUM)
        return PARSER_NOT_THIS;

    addNode (parent, inf->node);
    nextNode (inf);
    skipCharNext (inf);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------



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

    if (val == PUNCT_END_STR)
    {
        if ((node->type == NODE_TYPE_PUNCT &&
             node->value.punct == val) ||
            (node->type == NODE_TYPE_PUNCT &&
             node->value.punct == PUNCT_END_STR))
            return 1;
    }
    // tokenOneDump (node, "In checkPunct");
    if (node->type == NODE_TYPE_PUNCT &&
        node->value.punct == val)
        return 1;
    return 0;
}
// ---------------------------------------------------------------------------------------------------


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

    if (inf->node->type == NODE_TYPE_PUNCT &&
        inf->node->value.punct == PUNCT_END_STR)
        inf->line++;
    inf->node = inf->node + 1;
    inf->cur_index++;
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция скипа всех '\n'
 @param [in] inf Указатель на структуру контекста
*/
int skipCharEnd (ParserContextInf_t* inf)
{
    assert (inf);

    while (inf->node->type == NODE_TYPE_PUNCT &&
           inf->node->value.punct == PUNCT_END_STR)
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

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция скипа всех символов переноса на новую строку '\'
 * \param [in] inf Указатель на структуру контекста
 */
int skipCharNext (ParserContextInf_t* inf)
{
    assert (inf);

    // tokenOneDump (inf->node, "1");
    // tokenOneDump (inf->node + 1, "2");
    while (inf->node->type == NODE_TYPE_PUNCT &&
           inf->node->value.punct == PUNCT_NEXT_STR &&
           isEndChar (inf->node + 1))
    {
        if (inf->cur_index == inf->capacity)
            return 1;

        // tokenOneDump (inf->node, "NEXT");
        inf->node = inf->node + 2;
        inf->cur_index += 2;
        inf->line++;
    }
    return 0;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 * \brief Функция пропуска всех разделяющий символов (\n и \)
 * \param [in] inf Указатель на структуру контекста
*/
int skipVoid (ParserContextInf_t* inf)
{
    assert (inf);

    while ((inf->node->type == NODE_TYPE_PUNCT &&
            inf->node->value.punct == PUNCT_END_STR) ||
           (inf->node->type == NODE_TYPE_PUNCT &&
            inf->node->value.punct == PUNCT_NEXT_STR))
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

    while ((node->type == NODE_TYPE_PUNCT &&
            node->value.punct == PUNCT_END_STR) ||
           (node->type == NODE_TYPE_PUNCT &&
            node->value.punct == PUNCT_NEXT_STR))
        node = node + 1;

    return node;
}
// ---------------------------------------------------------------------------------------------------

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

        case PE_NOT_ERROR:
            break;

        default:
            printf ("Unhandled parser error (code: %d)", error);
    }

    printf("\n");
    return 0;
}
// ---------------------------------------------------------------------------------------------------
