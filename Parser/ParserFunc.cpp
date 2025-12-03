#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "SupportFunc.h"
#include "ParserFunc.h"
#include "TypesOfType.h"
#include "TokenFunc.h"
#include "DumpFunc.h"


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
        // tokenOneDump (inf.node);
        if (parserDeclarationFunc (&inf, global_node) == PARSER_THIS_OK)
            continue;
        // tokenOneDump (inf.node);

        if (parserDeclarationVar (&inf, global_node) == PARSER_THIS_OK)
            continue;
        // tokenOneDump (inf.node);

    // printf ("jjj\n");
        if (parserIfOper (&inf, global_node) == PARSER_THIS_OK)
            continue;
        // tokenOneDump (inf.node);
    // printf ("jjj\n");
        if (parserAssignCall (&inf, global_node) == PARSER_THIS_OK)
            continue;
        // tokenOneDump (inf.node);


        if (inf.node->type == NODE_TYPE_PUNCT &&
            inf.node->value.punct == PUNCT_END_STR)
        {
            nextNode (&inf);
            continue;
        }
        SYNTAX_ERROR (&inf, PE_UNKNOWN_VALUE);
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

    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_LEFT_ROUND)
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

    Node_t* block = inf->node;
    block->type = NODE_TYPE_BLOCK;
    addNode (node, block);
    nextNode (inf);
    // skipEndChar (inf);
    // printf ("666666666666\n");
    while (true)
    {
        skipEndChar (inf);
        // tokenOneDump (inf->node);
        if (parserDeclarationVar (inf, block) == PARSER_THIS_OK)
            continue;
        // tokenOneDump (inf->node);

        if (parserAssignCall (inf, block) == PARSER_THIS_OK)
        {
            // printf ("666666666666666666666666666666666\n");
            continue;

        }
        // tokenOneDump (inf->node);
        // tokenOneDump (inf->node);

        if (parserIfOper (inf, block) == PARSER_THIS_OK)
            continue;
        // tokenOneDump (inf->node);

        if (inf->node->type == NODE_TYPE_PUNCT &&
            inf->node->value.punct == PUNCT_END_STR)
        {
            nextNode (inf);
            continue;
        }
        break;
    }
    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_RIGHT_ROUND)
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

            // printf ("666666666666666666666666666666666\n");
    nextNode (inf);
    // skipEndChar (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания if () ()
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserIfOper (ParserContextInf_t* inf,
                       Node_t* node)
{
    assert (inf);
    assert (node);

    if (inf->node->type != NODE_TYPE_KEY_WORD ||
        inf->node->value.key != KEY_IF)
        return PARSER_NOT_THIS;

    Node_t* if_node = inf->node;
    // dumpNode (if_node);
    nextNode (inf);
    // skipEndChar (inf);
    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.key != PUNCT_LEFT_ROUND)
        SYNTAX_ERROR (inf, PE_NOT_LEFT_ROUND);

    // printf ("hhh\n");
    nextNode (inf);
    // skipEndChar (inf);
    if (parserExpresion (inf, if_node) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

    // printf ("ggg\n");
    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_RIGHT_ROUND)
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

    addNode (node, if_node);
    // dumpNode (node);
    // tokenOneDump (inf->node);
    nextNode (inf);
    skipEndChar (inf);
    if (parserUnion (inf, if_node) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_UNION);
    // tokenOneDump (inf->node);
    // dumpNode (if_node);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания приравнивания и вызова функции
 @param [in] inf Указатель    dumpNode (if_node);
 на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserAssignCall (ParserContextInf_t* inf,
                           Node_t* node)
{
    assert (inf);
    assert (node);

    if (inf->node->type != NODE_TYPE_INDENT ||
        inf->node->value.name == NULL)
        return PARSER_NOT_THIS;

    Node_t* indent = inf->node;
    nextNode (inf);
    // skipEndChar (inf);
    if (inf->node->type == NODE_TYPE_KEY_WORD &&
        inf->node->value.key == KEY_ASSIGN)
    {
    // tokenOneDump (inf->node);
        addNode (node, inf->node);
        addNode (inf->node, indent);
        Node_t* assign = inf->node;

        nextNode (inf);
        // skipEndChar (inf);
        if (parserExpresion (inf, assign) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION_IN_ASSIGN);
        // printf ("Assign\n");
        // tokenOneDump (inf->node);
    }
    else if (inf->node->type == NODE_TYPE_PUNCT &&
             inf->node->value.punct == PUNCT_LEFT_ROUND)
    {
        nextNode (inf);
        // skipEndChar (inf);
        addNode (node, indent);
        if (inf->node->type != NODE_TYPE_PUNCT ||
            inf->node->value.punct != PUNCT_RIGHT_ROUND)
        {
            while (true)
            {
                // skipEndChar (inf);
                if (parserExpresion (inf, indent) != PARSER_THIS_OK)
                    SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
                if (inf->node->type == NODE_TYPE_PUNCT &&
                    inf->node->value.punct == PUNCT_COMMA)
                {
                    nextNode (inf);
                    continue;
                }
                break;
            }
        }
        if (inf->node->type != NODE_TYPE_PUNCT ||
            inf->node->value.punct != PUNCT_RIGHT_ROUND)
            SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);
        nextNode (inf);
        // skipEndChar (inf);
    }
    if (inf->node->type == NODE_TYPE_PUNCT &&
        inf->node->value.punct == PUNCT_END_STR)
    {
        // printf ("555555555555555555555555\n");
        nextNode (inf);
        // skipEndChar (inf);
        return PARSER_THIS_OK;
    }
    else
        SYNTAX_ERROR (inf, PE_NOT_EXPRESION);
}
// ---------------------------------------------------------------------------------------------------
/*
// ---------------------------------------------------------------------------------------------------
Status_t compilerExtern (ParserContextInf_t* inf,
                         Node_t* node)
{
    assert (inf);
    assert (node);

    if (inf->node->type != NODE_TYPE_KEY_WORD &&
        inf->node->value.key != KEY_EXTERN_VAR &&
        inf->node->value.key != KEY_EXTERN_FUNC)
        return PARSER_NOT_THIS;

    addNode (node, inf->node);
    Node_t* ext = inf->node;
    nextNode (inf);

    if (inf->node->type != NODE_TYPE_INDENT)
        SYNTAX_ERROR ();

    Node_t* indent = inf->node;
    nextNode (inf);
    if (ext->value.key == KEY_EXTERN_VAR)
    {
        if (inf->node->type == NODE_TYPE_KEY_WORD &&
            inf->node->value.key == KEY_ASSIGN)
        {
            addNode (ext, inf->node);
            addNode (inf->node, indent);
            nextNode (inf);
            if (parserExpresion (inf, inf->node - 1) != PARSER_THIS_OK)
                SYNTAX_ERROR ();
        }
        else    { addNode (ext, indent); }
    }
    else if (parent->value.key == KEY_EXTERN_FUNC)
    {
        if (inf->node->type != NODE_TYPE_PUNCT &&
            inf->node->value.punct != PUNCT_LEFT_ROUND)
            SYNTAX_ERROR ();

        addNode (parent, ext);
        nextNode (inf);

        while (true)
        {
            if (inf->node->type != NODE_TYPE_INDENT)
                break;

            addNode (ext, inf->node);
            nextNode (inf);
            if (inf->node->type == NODE_TYPE_PUNCT &&
                inf->node->value.punct == PUNCT_COMMA)
            {
                nextNode (inf);
                continue;
            }
            break;
        }
        if (inf->node->type != NODE_TYPE_PUNCT &&
            inf->node->value.punct != PUNCT_RIGHT_ROUND)
            SYNTAX_ERROR ();

        nextNode (inf);
    }
    if (inf->node.type != NODE_TYPE_PUCNT &&
        inf->node->value.punct != PUCNT_END_STR)
        SYNTAX_ERROR ();

    nextNode (inf);
    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------
*/
// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания объявления заголовка функции
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
        inf->node->value.key != KEY_EXTERN_FUNC)
        return PARSER_NOT_THIS;

    if ((inf->node + 1)->type != NODE_TYPE_INDENT ||
        (inf->node + 2)->type != NODE_TYPE_PUNCT ||
        (inf->node + 2)->value.punct != PUNCT_LEFT_ROUND)
        return PARSER_NOT_THIS;

    addNode (parent, inf->node);
    Node_t* ext = inf->node;
    nextNode (inf);
    // skipEndChar (inf);

    if (inf->node->type != NODE_TYPE_INDENT)
        SYNTAX_ERROR (inf, PE_NOT_INDENT_AFTER_DECLARATION);

    nextNode (inf);
    // skipEndChar (inf);
    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_LEFT_ROUND)
        SYNTAX_ERROR (inf, PE_NOT_LEFT_ROUND);

    addNode (parent, ext);
    nextNode (inf);
    // skipEndChar (inf);

    while (true)
    {
        skipEndChar (inf);
        if (inf->node->type != NODE_TYPE_INDENT)
            break;

        addNode (ext, inf->node);
        nextNode (inf);
        // skipEndChar (inf);
        if (inf->node->type == NODE_TYPE_PUNCT &&
            inf->node->value.punct == PUNCT_COMMA)
        {
            nextNode (inf);
            continue;
        }
        break;
    }
    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_RIGHT_ROUND)
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

    nextNode (inf);
    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.key != PUNCT_END_STR)
        SYNTAX_ERROR (inf, PE_NOT_END_CHAR);

    nextNode (inf);
    // skipEndChar (inf);
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
        inf->node->value.key != KEY_EXTERN_VAR)
        return PARSER_NOT_THIS;

    addNode (node, inf->node);
    Node_t* ext = inf->node;
    nextNode (inf);
    // skipEndChar (inf);

    if (inf->node->type != NODE_TYPE_INDENT)
        SYNTAX_ERROR (inf, PE_NOT_INDENT_AFTER_DECLARATION);


    Node_t* indent = inf->node;
    nextNode (inf);
    // skipEndChar (inf);
    if (inf->node->type == NODE_TYPE_KEY_WORD &&
        inf->node->value.key == KEY_ASSIGN)
    {
        addNode (ext, inf->node);
        addNode (inf->node, indent);
        nextNode (inf);
        // skipEndChar (inf);
    // tokenOneDump (inf->node);
        if (parserExpresion (inf, inf->node - 1) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION_IN_ASSIGN);
    }
    else    { addNode (ext, indent); }

    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_END_STR)
        SYNTAX_ERROR (inf, PE_NOT_END_CHAR);

    nextNode (inf);
    // skipEndChar (inf);
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

    // printf ("EXP\n");
    Node_t* buffer_node = newNode ();
    if (parserAddSub (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);
    }

    // dumpNode (buffer_node);
    if (inf->node->type != NODE_TYPE_OPER ||
       (inf->node->value.oper != OPER_COMP_BIG_EQUAL &&
        inf->node->value.oper != OPER_COMP_ONLY_BIG &&
        inf->node->value.oper != OPER_COMP_LIT_EQUAL &&
        inf->node->value.oper != OPER_COMP_ONLY_LIT &&
        inf->node->value.oper != OPER_COMP_EQUAL &&
        inf->node->value.oper != OPER_COMP_NOT_EQUAL))
    {
    // dumpNode (node);
    // tokenOneDump (inf->node);
    // printf ("Exp\n");
    // tokenOneDump (inf->node);
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    // skipEndChar (inf);

    if (parserAddSub (inf, inf->node - 1) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);

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

    // tokenOneDump (inf->node);
    Node_t* buffer_node = newNode ();
    if (parserMulDiv (inf, buffer_node) != PARSER_THIS_OK)
    {
    // printf ("ADD\n");
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);
    }

    if (inf->node->type != NODE_TYPE_OPER ||
        inf->node->value.oper != OPER_ADD ||
        inf->node->value.oper != OPER_SUB)
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    // skipEndChar (inf);
    // tokenOneDump (inf->node);
    if (parserAddSub (inf, inf->node - 1) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);

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

    // tokenOneDump (inf->node);
    Node_t* buffer_node = newNode ();
    if (parserPower (inf, buffer_node) != PARSER_THIS_OK)
    {
    // printf ("MUL\n");
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);
    }

    if (inf->node->type != NODE_TYPE_OPER ||
        inf->node->value.oper != OPER_MUL ||
        inf->node->value.oper != OPER_DIV)
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    // skipEndChar (inf);

    if (parserMulDiv (inf, inf->node - 1) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);

    return PARSER_THIS_OK;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/**
 @brief Функция считывания значений в степени
 @param [in] inf Указатель на структуру с текущим положением
 @param [in] node Указатель на родителя поддерева
 @return PARSER_THIS_OK - если все нормально и это нужное место
         PARSER_NOT_THIS - если это не то место
         PARSER_ERROR - если произошла ошибка
*/
Status_t parserPower (ParserContextInf_t* inf,
                      Node_t* node)
{
    assert (inf);
    assert (node);

    // tokenOneDump (inf->node);
    Node_t* buffer_node = newNode ();
        // printf ("VAlue\n");
    if (parserValue (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);
    }

    if (inf->node->type != NODE_TYPE_OPER ||
        inf->node->value.oper != OPER_POW)
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    addChildren (inf->node, buffer_node);
    nextNode (inf);
    // skipEndChar (inf);
    if (parserValue (inf, inf->node - 1) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);

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

    // tokenOneDump (inf->node);
    if (inf->node->type == NODE_TYPE_PUNCT &&
        inf->node->value.punct == PUNCT_LEFT_ROUND)
    {
    // tokenOneDump (inf->node);
        nextNode (inf);
        // skipEndChar (inf);
        if (parserExpresion (inf, node) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

        if (inf->node->type == NODE_TYPE_PUNCT &&
            inf->node->value.punct == PUNCT_RIGHT_ROUND)
            SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

        nextNode (inf);
        // skipEndChar (inf);
        return PARSER_THIS_OK;
    }
    // tokenOneDump (inf->node);
    if (parserNumber (inf, node) == PARSER_THIS_OK)
        return PARSER_THIS_OK;
    // printf ("HHH\n");
    if (parserIndent (inf, node) == PARSER_THIS_OK)
        return PARSER_THIS_OK;

    SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);
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

    addNode (parent, inf->node);
    nextNode (inf);
    // skipEndChar (inf);

    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_LEFT_ROUND)
    {
        (inf->node - 1)->type = NODE_TYPE_VAR;
        return PARSER_THIS_OK;
    }

    (inf->node - 1)->type = NODE_TYPE_FUNC;
    nextNode (inf);
    // skipEndChar (inf);

    if (inf->node->type == NODE_TYPE_PUNCT &&
        inf->node->value.oper == PUNCT_RIGHT_ROUND)
    {
        nextNode (inf);
        // skipEndChar (inf);
        return PARSER_THIS_OK;
    }

    while (true)
    {
        if (parserExpresion (inf, inf->node) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION_IN_FUNC);

        if (inf->node->type == NODE_TYPE_PUNCT &&
            inf->node->value.punct == PUNCT_COMMA)
        {
            nextNode (inf);
            // skipEndChar (inf);
            continue;
        }
        break;
    }
    if (inf->node->type != NODE_TYPE_PUNCT ||
        inf->node->value.punct != PUNCT_RIGHT_ROUND)
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

    nextNode (inf);
    // skipEndChar (inf);
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
    // tokenOneDump (inf->node);

    if (inf->node->type != NODE_TYPE_NUM)
        return PARSER_NOT_THIS;

    addNode (parent, inf->node);
    nextNode (inf);
    // skipEndChar (inf);

    return PARSER_THIS_OK;
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
 @brief Функция смены текущего узла в контексте
 @param [in] inf Указатель на структуру контекста
*/
int nextNode (ParserContextInf_t* inf)
{
    assert (inf);

    if (inf->cur_index == inf->capacity)
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
int skipEndChar (ParserContextInf_t* inf)
{
    assert (inf);

    while (inf->node->type == NODE_TYPE_PUNCT &&
           inf->node->value.punct == PUNCT_END_STR)
    {
        if (inf->cur_index == inf->capacity)
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
        case (PE_NOT_INDENT_IN_FUNC):
        {
            printf ("Too few arguments in function");
            break;
        }
        case (PE_NOT_RIGHT_ROUND):
        {
            printf ("Absence '(')");
            break;
        }
        case (PE_NOT_LEFT_ROUND):
        {
            printf ("Absence '('");
            break;
        }
        case (PE_UNKNOWN_VALUE):
        {
            printf ("Unknown value");
            break;
        }
        case (PE_NOT_EXPRESION_AFTER_OPER):
        {
            printf ("No expresion");
            break;
        }
        case (PE_NOT_EXPRESION_IN_FUNC):
        {
            printf ("No expresion in function");
            break;
        }
        case (PE_NOT_INDENT_AFTER_DECLARATION):
        {
            printf ("No indent after declaration");
            break;
        }
        case (PE_NOT_EXPRESION_IN_ASSIGN):
        {
            printf ("No expresion after = ");
            break;
        }
        case (PE_NOT_END_CHAR):
        {
            printf ("No end char");
            break;
        }
        case (PE_NOT_ERROR): break;
        case (PE_NOT_EXPRESION):
        {
            printf ("No expresion");
            break;
        }
        case (PE_NOT_UNION):
        {
            printf ("No union");
            break;
        }
        default:
        {
            printf ("Unknown error");
        }
    }
    printf ("\n");
    return 0;
}
// ---------------------------------------------------------------------------------------------------
