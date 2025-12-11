#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "NodeType.h"
#include "TypesOfType.h"
#include "NameTableFunc.h"

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
 Когда на возвращяемся из функции, нам надо провернуть обратную операцию - а именно:
    Уменьший адрес в ВХ на значение в АХ
    Уменьшить АХ

 Для добавления значения переменных, лежащих по каким то адресам используется регистр FX

 Сейчас заняты:
    AX
    BX
    FX
  */
// ---------------------------------------------------------------------------------------------------




struct CompilerContextInf_t
{
    NameTableStack_t* stack;
    NameTable_t* table_func;
    FILE* stream;
    int amount_word;
    // int free_memory;
};

enum CompilerStatus_t
{
    CMP_THIS_OK,
    CMP_NOT_THIS
};

typedef int Memory_t;


CompilerStatus_t compilerBlock (CompilerContextInf_t* inf,
                                Node_t* node_block)
{
    assert (inf);
    assert (node_block);

    if (node_block->type != NODE_TYPE_BLOCK)
        return CMP_NOT_THIS;

    NameTable_t new_table = {};
    nameTableCtr (&new_table);
    nameTableStackPush (inf->stack, &new_table);

    for (int i = 0; i < node_block->amount_children; i++)
    {
        Node_t* node = node_block->children[i];
        if (node->type == NODE_TYPE_KEY_WORD &&
            node->value.key == KEY_EXTERN_VAR)
        {
            if (compilerExtVar (inf, node) != CMP_THIS_OK)
                COMPILER_ERROR ();
        }
        else if (node->type == NODE_TYPE_KEY_WORD &&
                 node->value.key == KEY_ASSIGN)
    }

}


// --------------------------------------------------------------------------------------------------
CompilerStatus_t compilerExtVar (CompilerContextInf_t* inf,
                                 Node_t* node_extern)
{
    assert (inf);
    assert (node_extern);

    if (node_extern->type != NODE_TYPE_KEY_WORD ||
        node_extern->value.key != KEY_EXTERN_VAR)
        return CMP_NOT_THIS;

    for (int i = 0; i < node_extern->amount_children; i++)
    {
        Node_t* node_var = node_extern->children[i];
        if (node_var->type == NODE_TYPE_VAR)
        {
            NameTableVar_t* add_var = compilerAddVar (inf, node_var->value.name);
            if (add_var == NULL)
                COMPILER_ERROR ();

            printAsm (inf, "PUSH 0\n");
            popValueStackToVar (inf, add_var);
        }
        else if (node_var->type == NODE_TYPE_KEY_WORD &&
                 node_var->value.key == KEY_ASSIGN)
        {
            node_var = node_var->children[0];
            NameTableVar_t* add_var = compilerAddVar (inf, node_var->value.name);
            if (add_var == NULL)
                COMPILER_ERROR ();

            node_var = node_var->parent->children[1];
            compilerOper (inf, node_var);
            popValueStackToVar (inf, add_var);
        }
        else
            COMPILER_ERROR ();
    }
    return CMP_THIS_OK;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int compilerIf (CompilerContextInf_t* inf,
                Node_t* node_key)
{
    assert (inf);
    assert (node_key);

    if (node_key->type != NODE_TYPE_KEY_WORD ||
        node_key->value.key != KEY_IF)
        return 0;

    if (node_key->amount_children != 2)
        COMPILER_ERROR ();

    compilerOper (inf, node_key->children[0]);
    printAsm (inf, "PUSH 0\n");
    printAsm (inf, "JE");

    long int jmp_to_else = ftell (inf->stream);
    fseek (inf->stream, 6, SEEK_CUR);
    printAsm (inf, "\n");
    compilerBlock (inf, node_key->children[1]);
    if ()

}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
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
    vfprintf(inf->stream, format, args);
    va_end(args);

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int compilerKeyWord (CompilerContextInf_t* inf,
                     Node_t* node)
{
    assert (inf);
    assert (node);

    if (node->type != NODE_TYPE_KEY_WORD)
        return 0;

    if (node->value.key == KEY_)
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int compilerOper (CompilerContextInf_t* inf,
                  Node_t* node)
{
    assert (inf);
    assert (node);

    if (node->type == NODE_TYPE_OPER)
    {
        if (node->amount_children != 2)
            COMPILER_ERROR ();

        compilerOper (inf, node->children[0]);
        compilerOper (inf, node->children[1]);

        switch (node->value.oper)
        {
            case (OPER_ADD): { printAsm (inf,"ADD\n"); break; }
            case (OPER_SUB): { printAsm (inf,"SUB\n"); break; }
            case (OPER_MUL): { printAsm (inf,"MUL\n"); break; }
            case (OPER_DIV): { printAsm (inf,"DIV\n"); break; }
            case (OPER_POW): { printAsm (inf,"POW\n"); break; }

            case (OPER_COMP_BIG_EQUAL): { compareValue (node->value.oper, inf->stream); break; }
            case (OPER_COMP_ONLY_BIG):  { compareValue (node->value.oper, inf->stream); break; }
            case (OPER_COMP_LIT_EQUAL): { compareValue (node->value.oper, inf->stream); break; }
            case (OPER_COMP_ONLY_LIT):  { compareValue (node->value.oper, inf->stream); break; }
            case (OPER_COMP_EQUAL):     { compareValue (node->value.oper, inf->stream); break; }
            case (OPER_COMP_NOT_EQUAL): { compareValue (node->value.oper, inf->stream); break; }

            default: { COMPILER_ERROR (); }
        }
    }
    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int compareValue (CompilerContextInf_t* inf,
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
        COMPILER_ERROR ();

    printAsm (inf, " $3\n");
    printAsm (inf, "PUSH 0\n");
    printAsm (inf, "JMP $1\n");
    printAsm (inf, "PUSH 1\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
NameTableVar_t* compilerAddVar (CompilerContextInf_t* inf,
                                char* name)
{
    assert (inf);
    assert (name);

    NameTableVar_t* var = compilerFindVar (inf->stack, name);
    if (var != NULL)
        COMPILER_ERROR ();

    NameTable_t* table_var = nameTableStackPop (inf->stack);
    var = nameTableAdd (table_var, name);
    nameTableStackPush (inf->stack, table_var);

    var->val.phase = table_var->size - 1;
    return var;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
NameTableVar_t* compilerFindVar (NameTableStack_t* stack,
                                 char* name)
{
    assert (stack);
    assert (name);

    NameTableVar_t* var = NULL;

    for (size_t i = 0; i < nameTableStackGetSize (stack); i++)
    {
        NameTable_t* table = nameTableStackGetEl (stack, i);
        var = nameTableFind (table, name);

        if (var == NULL)
            continue;
    }
    return var;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int pushValueVarToStack (CompilerContextInf_t* inf,
                         NameTableVar_t* var)
{
    assert (inf);
    assert (var);

    printAsm (inf, "PUSHR FX\n");
    printAsm (inf, "PUSH %d\n", var->val.phase);
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR FX\n");
    printAsm (inf, "PUSHM FX\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int popValueStackToVar (CompilerContextInf_t* inf,
                        NameTableVar_t* var)
{
    assert (inf);
    assert (var);

    printAsm (inf, "PUSHR FX\n");
    printAsm (inf, "PUSH %d\n", var->val.phase);
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR FX\n");
    printAsm (inf, "POPM FX\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int saveStatusFunc (CompilerContextInf_t* inf,
                    int amount_var)
{
    assert (inf);

    printAsm (inf, "PUSHM AX\n");
    printAsm (inf, "PUSHR BX\n");
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR BX\n");
    printAsm (inf, "PUSHR AX\n");
    printAsm (inf, "PUSH 1\n");
    printAsm (inf, "ADD\n");
    printAsm (inf, "POPR AX\n");
    printAsm (inf, "PUSH %d\n", amount_var);
    printAsm (inf, "POPM AX\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------
int replaceStatusToPrev (CompilerContextInf_t* inf)
{
    assert (inf);

    printAsm (inf, "PUSHR BX\n");
    printAsm (inf, "PUSHM AX\n");
    printAsm (inf, "SUB\n");
    printAsm (inf, "POPM BX\n");
    printAsm (inf, "PUSHR AX\n");
    printAsm (inf, "PUSH 1\n");
    printAsm (inf, "SUB\n");
    printAsm (inf, "POPR AX\n");

    return 0;
}
// --------------------------------------------------------------------------------------------------





