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
        skipVoid (&inf);
        if (parserDeclarationFunc (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (parserDeclarationVar (&inf, global_node) == PARSER_THIS_OK)
            continue;

        if (inf.node->type == NODE_TYPE_INDENT)
        {
            if (isLeftTang (inf.node))
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
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

    Node_t* block = inf->node;
    block->type = NODE_TYPE_BLOCK;
    addNode (node, block);
    nextNode (inf);

    while (true)
    {
        skipVoid (inf);
        if (parserDeclarationVar (inf, block) == PARSER_THIS_OK)
            continue;

        if (inf->node->type == NODE_TYPE_INDENT)
        {
            if (isLeftTang (inf->node))
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
        break;
    }
    if (!isRightTang (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);
    nextNode (inf);

    if (!isEndChar (inf->node))
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
    // tokenOneDump (if_node, "Зашли в if");
    nextNode (inf);
    skipVoid (inf);

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

    if (parserUnion (inf, cond_node) != PARSER_THIS_OK)
        SYNTAX_ERROR (inf, PE_NOT_UNION);

    if (!isEndChar (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_END_CHAR);

    skipVoid (inf);
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

        if (!isEndChar (inf->node))
            SYNTAX_ERROR (inf, PE_NOT_END_CHAR);

        return PARSER_THIS_OK;
    }
    else if (isEndChar (inf->node))
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

    Node_t* node_define = inf->node;
    addNode (parent, node_define);
    nextNode (inf);
    skipCharNext (inf);

    if (inf->node->type != NODE_TYPE_INDENT ||
        !isLeftTang (getNextNotEndNode (inf)))
        SYNTAX_ERROR (inf, PE_NOT_INDENT);

    addNode (node_define, inf->node);
    nextNode (inf);
    nextNode (inf);

    while (true)
    {
        skipVoid (inf);
        if (inf->node->type == NODE_TYPE_INDENT)
        {
            addNode (node_define, inf->node);
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
        SYNTAX_ERROR ();

    skipVoid (inf);
    if (isLeftRound (inf->node))
    {
        if (parserUnion (inf, node_define) != PARSER_THIS_OK)
            SYNTAX_ERROR ();
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
        SYNTAX_ERROR (inf, PE_NOT_INDENT_AFTER_DECLARATION);

    Node_t* indent = inf->node;
    nextNode (inf);
    // skipCharEnd (inf);
    if (inf->node->type == NODE_TYPE_KEY_WORD &&
        inf->node->value.key == KEY_ASSIGN)
    {
        addNode (ext, inf->node);
        addNode (inf->node, indent);
        nextNode (inf);
        skipNextChar (inf);
        // skipCharEnd (inf);
    // tokenOneDump (inf->node);
        if (parserExpresion (inf, inf->node - 1) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION_IN_ASSIGN);
    }
    else    { addNode (ext, indent); }

    if (!isEndChar (inf->node))
        SYNTAX_ERROR (inf, PE_NOT_END_CHAR);

    nextNode (inf);
    skipVoid (inf);
    // skipCharEnd (inf);
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
    skipNextChar (inf);
    // skipCharEnd (inf);

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
       (inf->node->value.oper != OPER_ADD &&
        inf->node->value.oper != OPER_SUB))
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    // tokenOneDump (inf->node, "Операция слож");
    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    skipNextChar (inf);
    // skipCharEnd (inf);
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
    if (parserValue (inf, buffer_node) != PARSER_THIS_OK)
    {
        deleteOneNode (buffer_node);
        SYNTAX_ERROR (inf, PE_UNKNOWN_VALUE);
    }

    if (inf->node->type != NODE_TYPE_OPER ||
       (inf->node->value.oper != OPER_MUL &&
        inf->node->value.oper != OPER_DIV))
    {
        addChildren (node, buffer_node);
        deleteOneNode (buffer_node);
        return PARSER_THIS_OK;
    }

    // printf ("MUL\n");
    addChildren (inf->node, buffer_node);
    deleteOneNode (buffer_node);
    addNode (node, inf->node);
    nextNode (inf);
    skipNextChar (inf);
    // skipCharEnd (inf);

    if (parserMulDiv (inf, inf->node - 1) != PARSER_THIS_OK)
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
    if (isLeftRound (inf->node))
    {
    // tokenOneDump (inf->node);
        nextNode (inf);
        skipVoid (inf);
        // skipCharEnd (inf);
        if (parserExpresion (inf, node) != PARSER_THIS_OK)
            SYNTAX_ERROR (inf, PE_NOT_EXPRESION);

        skipVoid (inf);
        if (!isRightRound (inf->node))
            SYNTAX_ERROR (inf, PE_NOT_RIGHT_ROUND);

        nextNode (inf);
        skipCharNext (inf);
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

    if (isLeftRound (getNextNotEndNode(inf)))
    {
        if (parserCallFunc (inf, parent) != PARSER_THIS_OK)
            SYNTAX_ERROR ();
    }
    else
    {
        if (parserVar (inf, parent) != PARSER_THIS_OK)
            SYNTAX_ERROR ();
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

    Node_t* node_func = inf->node;
    addNode (parent, node_func);
    node_func->type = NODE_TYPE_FUNC;
    nextNode (inf);
    nextNode (inf); // Skip '('

    while (true)
    {
        skipVoid (inf);
        if (parserExpresion (inf, node_func) != PARSER_THIS_OK)
            SYNTAX_ERROR ();

        skipVoid (inf);
        if (isComma (inf->node))
            break;
        nextNode (inf);
        continue;
    }
    if (!isLeftRound (inf->node))
        SYNTAX_ERROR ();
    nextNode (inf);

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

    // tokenOneDump (inf->node, "Число");
    addNode (parent, inf->node);
    nextNode (inf);
    skipCharNext (inf);
    // skipCharEnd (inf);

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
    if (node->type == NODE_TYPE_PUNCT &&
        node->value.punct == val)
        return 1;
    return 0;
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
int skipCharEnd (ParserContextInf_t* inf)
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
 * \brief Функция скипа всех символов переноса на новую строку '\'
 * \param [in] inf Указатель на структуру контекста
 */
int skipCharNext (ParserContextInf_t* inf)
{
    assert (inf);

    while (inf->node->type == NODE_TYPE_PUNCT &&
           inf->node->value.punct == PUNCT_NEXT_STR)
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
 @brief Функция выдачи следующего неконечного контекстного токена
 @param [in] inf Указатель на структуру контекста
 @return Узел
*/
Node_t* getNextNotEndNode (ParserContextInf_t* inf)
{
    assert (inf);
    Node_t* node = inf->node;

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
