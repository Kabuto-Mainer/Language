#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "CompilerFunc.h"
#include "NodeType.h"
#include "CompilerType.h"
#include "TypesOfType.h"
#include "NameTableFunc.h"
#include "SupportFunc.h"

// ---------------------------------------------------------------------------------------------------
/*
 Язык функциональный -> никаких глобальных переменных. Все через аргументы
 Весь код написанный все функции выполнится
 ```Фактически, весь файл - это main, но в котором можно объявлять функции```
*/
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
/* Схема работы памяти
 У нас есть два стека памяти
  1 - отвечает за хранение всех переменных у функций, которые сейчас вызваны
  2 - отвечает за хранения количества переменных каждой функции
 Адрес хранения вершины 2 стека лежит в регистре АХ
 В регистре ВХ хранится адрес начала текущей функции
 Когда мы в функции вызываем функцию - нам следует сохранить состояние текущей
 Тогда мы смещаем адрес в ВХ на количество переменных функции, из которой мы делаем вызов
 (как раз это количество хранится на вершине стека, с которым связан регистр АХ)
 Адрес в регистре АХ мы увеличиваем на 1 и записываем туда количество переменных функции, которую мы вызвали
 Когда мы возвращаемся из функции, нам надо провернуть обратную операцию - а именно:
    Уменьшить адрес в ВХ на значение в АХ
    Уменьшить АХ

 Для добавления значения переменных, лежащих по каким-то адресам используется регистр FX
 Все возвращаемые значения лежат в регистре CX

 Сейчас заняты:
    AX
    BX
    CX
    FX
  */
// ---------------------------------------------------------------------------------------------------

#define COMPILER_ERROR(__inf__,__error__,__ret_val__)           \
    do {                                                        \
        printf ("ERROR: ");                                     \
        CompilerError (__error__);                              \
        printf ("\n```Call from %s:%d\n", __FILE__, __LINE__);  \
        return __ret_val__;                                     \
    } while (0)

// --------------------------------------------------------------------------------------------------
void fillSystemReg (FILE* stream)
{
    assert (stream);

    fprintf (stream, "; System start alloc memory\n");
    fprintf (stream, "PUSH 0\n");
    fprintf (stream, "POPR AX\n");
    fprintf (stream, "PUSH 100\n");
    fprintf (stream, "POPR BX\n");
    fprintf (stream, "; Start main code\n");
}
// --------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------
/**
 * \brief Глобальная функция ассемблирования
 * \param [in] name_file Имя файла
 * \param [in] root Корень дерева
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerGlobal (const char* name_file,
                                 Node_t* root)
{
    assert (name_file);
    assert (root);

    FILE* stream = fopen (name_file, "w");
    if (stream == NULL)
        EXIT_FUNC ("NULL file", CMP_ERROR);

    fillSystemReg (stream);
    // printf ("OPER FILE\n");
    NameTable_t table_func = {};
    nameTableCtr (&table_func);

    if (fillingTableFunc (&table_func, root) != 0)
    {
        nameTableDtr (&table_func);
        COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
    }

    NameTableStack_t stack = {};
    nameTableStackCtr (&stack);

    CompilerContextInf_t inf = {
        .stack = &stack,
        .table_func = &table_func,
        .stream = stream,
        .amount_word = 0,
        .free_point = 1
    };

    inf.num_while = StackCtr (1);

    if (compilerBlock (&inf, root) != CMP_THIS_OK)
    {
        nameTableStackDtr (&stack);
        StackDtr (inf.num_while);
        // nameTableDtr (&table_func);
        COMPILER_ERROR (&inf, CE_JUST_ERROR, CMP_ERROR);
    }

    StackDtr (inf.num_while);
    nameTableStackDtr (inf.stack);
    inf.num_while = NULL;
    inf.stack = NULL;

    printAsm (&inf, "HLT\n");
    printAsm (&inf, "\n; Start All Functions\n");

    if (compilerAllFunc (&inf, root) != CMP_THIS_OK)
    {
        // nameTableStackDtr (&stack);
        // StackDtr (inf.num_while);
        // nameTableDtr (&table_func);
        COMPILER_ERROR (&inf, CE_JUST_ERROR, CMP_ERROR);
    }
    // nameTableStackDtr (&stack);
    // StackDtr (inf.num_while);
    nameTableDtr (&table_func);
    // StackDtr(inf.num_while);
    fclose (stream);

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция перевода блока узлов в ассемблер
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_block Узел блока
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerBlock (CompilerContextInf_t* inf,
                                Node_t* node_block)
{
    assert (inf);
    assert (node_block);

    // printf ("Enter to block\n");

    if (node_block->type != NODE_TYPE_BLOCK)
        return CMP_NOT_THIS;

    NameTable_t new_table = {};
    nameTableCtr (&new_table);
    nameTableStackPush (inf->stack, &new_table);

    Node_t** nodes = node_block->children;
    int number_end_cond = 0;

    for (int i = 0; i < node_block->amount_children; i++)
    {
        Node_t* node = nodes[i];
        if (node->type == NODE_TYPE_FUNC)
        {
            if (compilerCallFunc (inf, node) != CMP_THIS_OK)
                COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
            continue;
        }

        if (node->type != NODE_TYPE_KEY_WORD)
            COMPILER_ERROR (inf, CE_NOT_KEY, CMP_ERROR);

        switch (node->value.key)
        {
            case (KEY_ASSIGN):
                // printf ("ASSIGN\n");
                if (compilerAssign (inf, node) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_ASSIGN_ER, CMP_ERROR);
                number_end_cond = 0;
                break;

            case (KEY_DEFINE_VAR):
                // printf ("DEFINE VAR\n");
                if (compilerExtVar (inf, node) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_VARDEF_ER, CMP_ERROR);
                number_end_cond = 0;
                break;

            case (KEY_DEFINE_FUNC):
                number_end_cond = 0;
                continue;

            case (KEY_IF):
                // printf ("IF\n");
                number_end_cond = inf->free_point++;
                if (compilerIf (inf, node, number_end_cond) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_COND_ER, CMP_ERROR);
                if (i + 1 < node_block->amount_children &&
                    (nodes[i+1]->type != NODE_TYPE_KEY_WORD ||
                    (nodes[i+1]->value.key != KEY_ELSE_IF &&
                     nodes[i+1]->value.key != KEY_ELSE)))
                    { printAsm (inf, "::sys%dtem\n", number_end_cond);
                      /*inf->free_point++;*/}
                break;

            case (KEY_ELSE_IF):
                // printf ("ELSE IF\n");
                if (number_end_cond == 0)
                    COMPILER_ERROR (inf, CE_ELSE_OUT_IF, CMP_ERROR);
                if (compilerElseIf (inf, node, number_end_cond) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_COND_ER, CMP_ERROR);
                if (i + 1 < node_block->amount_children &&
                    (nodes[i+1]->type != NODE_TYPE_KEY_WORD ||
                    (nodes[i+1]->value.key != KEY_ELSE_IF &&
                     nodes[i+1]->value.key != KEY_ELSE)))
                    printAsm (inf, "::sys%dtem\n", number_end_cond);
                break;

            case (KEY_ELSE):
                // printf ("ELSE\n");
                if (number_end_cond == 0)
                    COMPILER_ERROR (inf, CE_ELSE_OUT_IF, CMP_ERROR);
                if (compilerElse (inf, node, number_end_cond) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_COND_ER, CMP_ERROR);
                break;

            case (KEY_RETURN):
                // printf ("RETURN\n");
                if (compilerReturn (inf, node) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_RETURN, CMP_ERROR);
                break;

            case (KEY_WHILE):
                if (compilerWhile (inf, node) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_WHILE, CMP_ERROR);
                break;
            case (KEY_IN):
            case (KEY_OUT):
                if (compilerInOut (inf, node) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
                break;

            case (KEY_BREAK):
                if (inf->num_while->size != 0)
                {
                    int number = StackPop (inf->num_while);
                    printAsm (inf, "JMP ::sys%dtem\n", number);
                    StackPush (inf->num_while, number);
                    break;
                }
                COMPILER_ERROR (inf, CE_BREAK_OUT_WHILE, CMP_ERROR);

            case (KEY_PIXEL):
                if (compilerPixel (inf, node) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
                break;

            case (KEY_DRAW):
                if (compilerDraw (inf, node) != CMP_THIS_OK)
                    COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
                break;

            default:
                COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
        }
    }
    nameTableDtr (&new_table);
    nameTableStackPop (inf->stack);

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция добавления переменной в таблицу имен и её инициализация
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_extern Узел объявления переменной
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerExtVar (CompilerContextInf_t* inf,
                                 Node_t* node_extern)
{
    assert (inf);
    assert (node_extern);

    if (node_extern->type != NODE_TYPE_KEY_WORD ||
        node_extern->value.key != KEY_DEFINE_VAR)
        return CMP_NOT_THIS;

    // printf ("hgsvihgsvsvdvsd\n");
    for (int i = 0; i < node_extern->amount_children; i++)
    {
        Node_t* node_var = node_extern->children[i];
        if (node_var->type == NODE_TYPE_VAR)
        {
            NameTableVar_t* add_var = compilerAddVar (inf, node_var->value.name);
            if (add_var == NULL)
                COMPILER_ERROR (inf, CE_REDECLAR_VAR, CMP_ERROR);

            printAsm (inf, "PUSH 0\n");
            popValueStackToVar (inf, add_var);
        }
        else if (node_var->type == NODE_TYPE_KEY_WORD &&
                 node_var->value.key == KEY_ASSIGN)
        {
            // printf ("Assign\n");
            node_var = node_var->children[0];
            NameTableVar_t* add_var = compilerAddVar (inf, node_var->value.name);
            if (add_var == NULL)
                COMPILER_ERROR (inf, CE_REDECLAR_VAR, CMP_ERROR);

            node_var = node_var->parent->children[1];
            compilerExpresion (inf, node_var);
            popValueStackToVar (inf, add_var);
        }
        else
            COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
    }
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция обработки выхода из функции
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_return Узел return
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerReturn (CompilerContextInf_t* inf,
                                 Node_t* node_return)
{
    assert (inf);
    assert (node_return);

    if (node_return->amount_children != 1)
        COMPILER_ERROR (inf, CE_RETURN, CMP_ERROR);
    if (compilerExpresion (inf, node_return->children[0]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_EXPRESION, CMP_ERROR);

    printAsm (inf, "POPR CX\n");
    // printAsm (inf, "PUSHR CX\n");
    replaceStatusToPrev (inf);
    printAsm (inf, "RET\n");

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования цикла while
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_while Узел while
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerWhile (CompilerContextInf_t* inf,
                                Node_t* node_while)
{
    assert (inf);
    assert (node_while);

    if (node_while->amount_children != 2)
        COMPILER_ERROR (inf, CE_CHILDREN, CMP_ERROR);

    int number_while = inf->free_point++;
    int number_end = inf->free_point++;
    printAsm (inf, "\n::sys%dtem\n", number_while);
    if (compilerExpresion (inf, node_while->children[0]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_EXPRESION, CMP_ERROR);

    printAsm (inf, "PUSH 0\n");
    printAsm (inf, "JE ::sys%dtem\n", number_end);

    StackPush (inf->num_while, number_end);
    if (compilerBlock (inf, node_while->children[1]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_BLOCK, CMP_ERROR);

    StackPop (inf->num_while);
    printAsm (inf, "JMP ::sys%dtem\n", number_while);
    printAsm (inf, "\n::sys%dtem\n", number_end);
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования IF
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_cond Узел if
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerIf (CompilerContextInf_t* inf,
                             Node_t* node_cond,
                             int number_end)
{
    assert (inf);
    assert (node_cond);

    if (node_cond->amount_children != 2)
        COMPILER_ERROR (inf, CE_CHILDREN, CMP_ERROR);

    if (compilerExpresion (inf, node_cond->children[0]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_EXPRESION, CMP_ERROR);

    printAsm (inf, "PUSH 0\n");

    int number_else = inf->free_point++;
    printAsm (inf, "JE ::sys%dtem\n", number_else);

    if (compilerBlock (inf, node_cond->children[1]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_BLOCK, CMP_ERROR);

    printAsm (inf, "JMP ::sys%dtem\n", number_end);
    printAsm (inf, "::sys%dtem\n", number_else);

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования ELSE IF
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_cond Узел elif
 * \param [in] number_end Номер метки для конца блока условий
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerElseIf (CompilerContextInf_t* inf,
                                 Node_t* node_cond,
                                 int number_end)
{
    assert (inf);
    assert (node_cond);

    if (node_cond->amount_children != 2)
        COMPILER_ERROR (inf, CE_CHILDREN, CMP_ERROR);

    // printAsm (inf, "::sys%dtem\n", inf->free_point++);
    if (compilerExpresion (inf, node_cond->children[0]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_EXPRESION, CMP_ERROR);

    printAsm (inf, "PUSH 0\n");

    int number_else = inf->free_point++;
    printAsm (inf, "JE ::sys%dtem\n", number_else);

    if (compilerBlock (inf, node_cond->children[1]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_BLOCK, CMP_ERROR);

    printAsm (inf, "JMP ::sys%dtem\n", number_end);
    printAsm (inf, "::sys%dtem\n", number_else);

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования ELSE
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_cond Узел else
 * \param [in] number_end Номер метки для конца условий
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerElse (CompilerContextInf_t* inf,
                               Node_t* node_cond,
                               int number_end)
{
    assert (inf);
    assert (node_cond);

    if (node_cond->amount_children != 1)
        COMPILER_ERROR (inf, CE_CHILDREN, CMP_ERROR);

    if (compilerBlock (inf, node_cond->children[0]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_BLOCK, CMP_ERROR);

    printAsm (inf, "::sys%dtem\n", number_end);
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования оператора присваивания
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_block Узел =
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerAssign (CompilerContextInf_t* inf,
                                 Node_t* node_assign)
{
    assert (inf);
    assert (node_assign);

    if (node_assign->amount_children != 2)
        COMPILER_ERROR (inf, CE_CHILDREN, CMP_ERROR);

    NameTableVar_t* var = compilerFindVar (inf->stack, node_assign->children[0]->value.name);
    if (var == NULL)
        COMPILER_ERROR (inf, CE_UNKNOWN_VAR, CMP_ERROR);

    if (compilerExpresion (inf, node_assign->children[1]) != CMP_THIS_OK)
        COMPILER_ERROR (inf, CE_EXPRESION, CMP_ERROR);

    popValueStackToVar (inf, var);
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования выражения
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node Узел выражения
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerExpresion (CompilerContextInf_t* inf,
                                    Node_t* node)
{
    assert (inf);
    assert (node);

    if (node->type == NODE_TYPE_OPER)
    {
        if (node->amount_children != 2)
            COMPILER_ERROR (inf, CE_CHILDREN, CMP_ERROR);

        if (compilerExpresion (inf, node->children[1]) != CMP_THIS_OK ||
            compilerExpresion (inf, node->children[0]) != CMP_THIS_OK ||
            compilerOper (inf, node) != CMP_THIS_OK)
            COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
    }
    else if (node->type == NODE_TYPE_NUM)
    {
        printAsm (inf, "PUSH %d\n", node->value.num);
    }
    else if (node->type == NODE_TYPE_VAR)
    {
        NameTableVar_t* var = compilerFindVar (inf->stack, node->value.name);
        if (var == NULL)
            COMPILER_ERROR (inf, CE_UNKNOWN_VAR, CMP_ERROR);
        pushValueVarToStack (inf, var);
    }
    else if (node->type == NODE_TYPE_FUNC)
    {
        if (compilerCallFunc (inf, node) != CMP_THIS_OK)
            COMPILER_ERROR (inf, CE_UNKNOWN_FUNC, CMP_ERROR);
    }
    else
        COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования вызова функции
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_func Узел функции
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerCallFunc (CompilerContextInf_t* inf,
                                   Node_t* node_func)
{
    assert (inf);
    assert (node_func);

    if (node_func->type != NODE_TYPE_FUNC)
        COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);

    NameTableVar_t* func = nameTableFind (inf->table_func, node_func->value.name);
    if (func == NULL)
        COMPILER_ERROR (inf, CE_UNKNOWN_FUNC, CMP_ERROR);

    if (func->val.agrc != node_func->amount_children)
        COMPILER_ERROR (inf, CE_AMOUNT_ARGS, CMP_ERROR);

    for (int i = 0; i < node_func->amount_children; i++)
    {
        if (compilerExpresion (inf, node_func->children[i]) != CMP_THIS_OK)
            COMPILER_ERROR (inf, CE_EXPRESION, CMP_ERROR);
    }

    // printf ("In call func\nVARS: %d\n", findAmountVars(inf));
    saveStatusFunc (inf, findAmountVars (inf));
    printAsm (inf, "CALL :%s\n", func->name);
    printAsm (inf, "PUSHR CX\n");

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования операторов
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node Узел операции
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerOper (CompilerContextInf_t* inf,
                               Node_t* node)
{
    assert (inf);
    assert (node);

    switch (node->value.oper)
    {
        case (OPER_ADD): { printAsm (inf,"ADD\n"); break; }
        case (OPER_SUB): { printAsm (inf,"SUB\n"); break; }
        case (OPER_MUL): { printAsm (inf,"MUL\n"); break; }
        case (OPER_DIV): { printAsm (inf,"DIV\n"); break; }
        // case (OPER_POW): { printAsm (inf,"POW\n"); break; }

        case (OPER_COMP_BIG_EQUAL): { compilerCompare (inf, node->value.oper); break; }
        case (OPER_COMP_ONLY_BIG):  { compilerCompare (inf, node->value.oper); break; }
        case (OPER_COMP_LIT_EQUAL): { compilerCompare (inf, node->value.oper); break; }
        case (OPER_COMP_ONLY_LIT):  { compilerCompare (inf, node->value.oper); break; }
        case (OPER_COMP_EQUAL):     { compilerCompare (inf, node->value.oper); break; }
        case (OPER_COMP_NOT_EQUAL): { compilerCompare (inf, node->value.oper); break; }

        default: { COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR); }
    }
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования операторов сравнения
 * \param [in] inf Указатель на структуру контекста
 * \param [in] oper Операция сравнения
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerCompare (CompilerContextInf_t* inf,
                                  int oper)
{
    assert (inf);

    if      (oper == OPER_COMP_BIG_EQUAL)   { printAsm (inf, "JAE"); }
    else if (oper == OPER_COMP_ONLY_BIG)    { printAsm (inf, "JA"); }
    else if (oper == OPER_COMP_LIT_EQUAL)   { printAsm (inf, "JBE"); }
    else if (oper == OPER_COMP_ONLY_LIT)    { printAsm (inf, "JB"); }
    else if (oper == OPER_COMP_EQUAL)       { printAsm (inf, "JE"); }
    else if (oper == OPER_COMP_NOT_EQUAL)   { printAsm (inf, "JNE"); }
    else
        COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);

    printAsm (inf, " $3\n");
    printAsm (inf, "PUSH 0\n");
    printAsm (inf, "JMP $1\n");
    printAsm (inf, "PUSH 1\n\n");

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования ключевого слова PIXEL
 * \param [in] inf Указатель на структуру контекста
 * \param [in] oper Операция сравнения
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerPixel (CompilerContextInf_t* inf,
                                Node_t* node)
{
    assert (inf);
    assert (node);

    for (int i = 0; i < node->amount_children; i++)
    {
        if (compilerExpresion (inf, node->children[i]) != CMP_THIS_OK)
            COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
    }
    printAsm (inf, "PAINT 0\n");
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования ключевого слова DRAW
 * \param [in] inf Указатель на структуру контекста
 * \param [in] oper Операция сравнения
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerDraw (CompilerContextInf_t* inf,
                               Node_t* node)
{
    assert (inf);
    assert (node);

    printAsm (inf, "DRAW\n");
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования всех функций
 * \param [in] inf Указатель на структуру контекста
 * \param [in] root Корень дерева
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerAllFunc (CompilerContextInf_t* inf,
                                  Node_t* root)
{
    assert (inf);
    assert (root);

    Node_t** nodes = root->children;
    for (int i = 0; i < root->amount_children; i++)
    {
        if (nodes[i]->type != NODE_TYPE_KEY_WORD ||
            nodes[i]->value.key != KEY_DEFINE_FUNC)
            continue;

        Node_t* func = nodes[i];
        if (func->children[func->amount_children - 1]->type != NODE_TYPE_BLOCK)
            continue;

        NameTableStack_t stack = {};
        nameTableStackCtr (&stack);
        if (inf->stack != NULL)
            nameTableStackDtr (inf->stack);
        inf->stack = &stack;

        NameTable_t table = {};
        nameTableCtr (&table);
        nameTableStackPush (&stack, &table);
        inf->num_while = StackCtr (1);

        printAsm (inf, ":%s\n", func->children[0]->value.name);
        for (int ch = func->amount_children - 2; ch > 0; ch--)
        {
            NameTableVar_t* var  = compilerAddVar (inf, func->children[ch]->value.name);
            popValueStackToVar (inf, var);
        }
        if (compilerBlock (inf, nodes[i]->children[nodes[i]->amount_children - 1]) != CMP_THIS_OK)
        {
            nameTableStackDtr (&stack);
            StackDtr (inf->num_while);
            COMPILER_ERROR (inf, CE_BLOCK, CMP_ERROR);
        }
        nameTableStackDtr (&stack);
        StackDtr (inf->num_while);
        inf->stack = NULL;
        inf->num_while = NULL;

        printAsm (inf, "HLT\n");
    }
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция ассемблирования in и out
 * \param [in] inf Указатель на структуру контекста
 * \param [in] node_key Узел с ключевым словом
 * \return CMP_THIS_OK, Если было успешно ассемблировано
           CMP_NOT_THIS, Если оказался не тот узел
           CMP_ERROR, Если произошла ошибка в ходе выполнения ассемблирования
 */
CompilerStatus_t compilerInOut (CompilerContextInf_t* inf,
                                Node_t* node_key)
{
    assert (inf);
    assert (node_key);

    if (node_key->type != NODE_TYPE_KEY_WORD ||
        (node_key->value.key != KEY_IN &&
         node_key->value.key != KEY_OUT))
        return CMP_NOT_THIS;

    if (node_key->amount_children != 1)
        COMPILER_ERROR (inf, CE_CHILDREN, CMP_ERROR);

    if (node_key->value.key == KEY_IN)
    {
        NameTableVar_t* var = compilerFindVar (inf->stack, node_key->children[0]->value.name);
        if (var == NULL)
            COMPILER_ERROR (inf, CE_UNKNOWN_VAR, CMP_ERROR);
        printAsm (inf, "IN\n");
        popValueStackToVar (inf, var);
    }
    else
    {
        if (compilerExpresion (inf, node_key->children[0]) != CMP_THIS_OK)
            COMPILER_ERROR (inf, CE_EXPRESION, CMP_ERROR);
        printAsm (inf, "OUT\n\n");
    }

    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи строки в ассемблерный файл
 * \param [in] inf Указатель на структуру контекста
 * \param [in] format Строка
 * \note Обертка на fprintf()
 */
int printAsm (CompilerContextInf_t* inf,
              const char* format, ... )
{
    assert (inf);
    assert (format);

    if (strchr (format, ' ') != NULL)
        inf->amount_word += 2;
    else
        inf->amount_word++;

    va_list args;
    va_start (args, format);
    vfprintf (inf->stream, format, args);
    va_end (args);

    return 0;
}
// --------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция добавления переменной в таблицу имен и присвоения ей её смещения
 * \param [in] inf Указатель на структуру контекста
 * \param [in] name Имя переменной
 * \return Указатель на структуру переменной
 * \note Смещение отсчитывается от начала первой таблице в стеке
 */
NameTableVar_t* compilerAddVar (CompilerContextInf_t* inf,
                                char* name)
{
    assert (inf);
    assert (name);

    NameTable_t* table = nameTableStackPop (inf->stack);

    if (nameTableFind (table, name) != NULL)
        COMPILER_ERROR (inf, CE_REDECLAR_VAR, NULL);

    NameTableVar_t* var = nameTableAdd (table, name);
    nameTableStackPush (inf->stack, table);

    int var_phase = 0;
    for (int i = (int) nameTableStackGetSize (inf->stack) - 2; i >= 0;  i--)
        var_phase += (int) nameTableGetSize (nameTableStackGetEl (inf->stack, ((size_t) i)));

    var_phase += (int) table->size - 1;

    var->val.phase = var_phase;
    return var;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция поиска переменной в всем стеке таблиц переменных
 * \param [in] stack Указатель на стек таблиц переменных
 * \param [in] name Имя переменной
 * \return Указатель на структуру переменной
 */
NameTableVar_t* compilerFindVar (NameTableStack_t* stack,
                                 char* name)
{
    assert (stack);
    assert (name);

    NameTableVar_t* var = NULL;

    if (nameTableStackGetSize (stack) == 0)
        return NULL;

    for (int i = ((int) nameTableStackGetSize (stack)) - 1; i >= 0; i--)
    {
        NameTable_t* table = nameTableStackGetEl (stack, (size_t) i);
        var = nameTableFind (table, name);

        if (var == NULL)
            continue;
        break;
    }
    return var;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи ассемблерного кода пуша значения переменной в стек
 * \param [in] inf Указатель на структуру контекста
 * \param [in] var Указатель на структуру переменной
 */
int pushValueVarToStack (CompilerContextInf_t* inf,
                         NameTableVar_t* var)
{
    assert (inf);
    assert (var);

    // printf ("VAR: %s [%d]\n", var->name, var->val.phase);
    printAsm (inf, "PUSHR BX\n");
    printAsm (inf, "PUSH %d\n", var->val.phase);
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR FX\n");
    printAsm (inf, "PUSHM FX\n\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи ассемблерного кода записи значения из стека в переменную
 * \param [in] inf Указатель на структуру контекста
 * \param [in] var Указатель на структуру контекста
 */
int popValueStackToVar (CompilerContextInf_t* inf,
                        NameTableVar_t* var)
{
    assert (inf);
    assert (var);

    printAsm (inf, "PUSHR BX\n");
    printAsm (inf, "PUSH %d\n", var->val.phase);
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR FX\n");
    printAsm (inf, "POPM FX\n\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция записи ассемблерного кода сохранения состояния функции
 * \param [in] inf Указатель на структуру контекста
 * \param [in] amount_var Количество переменных, значения которых нужно сохранить
 */
int saveStatusFunc (CompilerContextInf_t* inf,
                    int amount_var)
{
    assert (inf);

    printAsm (inf, "PUSHR AX\n");
    printAsm (inf, "PUSH 1\n");
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR AX\n");
    printAsm (inf, "PUSHR BX\n");
    printAsm (inf, "POPM AX\n\n");
    printAsm (inf, "PUSH %d\n", amount_var);
    printAsm (inf, "PUSHR BX\n");
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR BX\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция возращения стека вызовов функций в предыдущее положение
 * \param [in] inf Указатель на структуру контекста
 */
int replaceStatusToPrev (CompilerContextInf_t* inf)
{
    assert (inf);

    printAsm (inf, "PUSHM AX\n");
    printAsm (inf, "POPR BX\n");
    printAsm (inf, "PUSH 1\n");
    printAsm (inf, "PUSHR AX\n");
    printAsm (inf, "SUB\n");
    printAsm (inf, "POPR AX\n\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция заполнения таблицы функций, находя количество их аргументов
 * \param [in] table_func Указатель на структуру контекста
 * \param [in] root Коренной узел дерева
 */
int fillingTableFunc (NameTable_t* table_func,
                      Node_t* root)
{
    assert (table_func);
    assert (root);

    Node_t** nodes = root->children;
    for (int i = 0; i < root->amount_children; i++)
    {
        if (nodes[i]->type != NODE_TYPE_KEY_WORD ||
            nodes[i]->value.key != KEY_DEFINE_FUNC)
            continue;

        Node_t* node_func = nodes[i]->children[0];
        int amount_args = 0;
        for (int ch = 1; ch < nodes[i]->amount_children; ch++)
        {
            if (nodes[i]->children[ch]->type == NODE_TYPE_VAR)
            {
                amount_args++;
                continue;
            }
            if (nodes[i]->children[ch]->type == NODE_TYPE_BLOCK)
                break;

            COMPILER_ERROR (inf, CE_JUST_ERROR, CMP_ERROR);
        }

        NameTableVar_t* var = nameTableFind (table_func, node_func->value.name);
        if (var != NULL && var->val.agrc != amount_args)
            COMPILER_ERROR (inf, CE_AMOUNT_ARGS, CMP_ERROR);

        if (var == NULL)
        {
            var = nameTableAdd (table_func, node_func->value.name);
            var->val.agrc = amount_args;
        }
        // printf ("FUNC: %s (%d)\n", var->name, var->val.agrc);
    }
    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
/**
 * \brief Функция нахождения текущего количество переменных в стеке таблиц переменных
 * \param [in] inf Указатель на структуру контекста
 * \return Количество переменных в стеке
 */
int findAmountVars (CompilerContextInf_t* inf)
{
    assert (inf);

    int size = 0;
    for (int i = ((int) nameTableStackGetSize (inf->stack)) - 1; i >= 0; i--)
    {
        NameTable_t* table = nameTableStackGetEl (inf->stack, (size_t) i);
        size += (int) nameTableGetSize (table);
    }
    return size;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
void CompilerError (CompilerError_t error)
{
    switch (error)
    {
        case CE_JUST_ERROR:
            printf ("Just buffer error. Check prevs");
            break;
        case CE_NOT_KEY:
            printf ("Not key word");
            break;
        case CE_ASSIGN_ER:
            printf ("Buffer error in assign");
            break;
        case CE_VARDEF_ER:
            printf ("Buffer error in define variable");
            break;
        case CE_COND_ER:
            printf ("Problem with transpond cond");
            break;
        case CE_ELSE_OUT_IF:
            printf ("Not prev if for this cond");
            break;
        case CE_RETURN:
            printf ("Problem with return");
            break;
        case CE_WHILE:
            printf ("Problem with while");
            break;
        case CE_REDECLAR_VAR:
            printf ("Redeclaration variable");
            break;
        case CE_EXPRESION:
            printf ("Problem with expresion");
            break;
        case CE_BLOCK:
            printf ("Problem with block");
            break;
        case CE_CHILDREN:
            printf ("Incorrect amount children");
            break;
        case CE_UNKNOWN_VAR:
            printf ("Unknown variable");
            break;
        case CE_UNKNOWN_FUNC:
            printf ("Unknown function");
            break;
        case CE_AMOUNT_ARGS:
            printf ("Incorrect amount arguments for function");
            break;
        case CE_BREAK_OUT_WHILE:
            printf ("Incorrect call break without while");
            break;;

        default:
            break;
    }
}
// --------------------------------------------------------------------------------------------------
