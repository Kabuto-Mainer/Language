#include <assert.h>

#include "NodeType.h"
#include "TypesOfType.h"
#include "NameTableFunc.h"

struct CompilerContextInf_t
{
    NameTableStack_t* stack;
    NameTable_t* table_func;
    FILE* stream;
    int free_memory;
};

typedef int Memory_t;
// int compilerGlobal (Node_t* root,
//                     NameTableStack_t* stack,
//                     NameTable_t* table_func,
//                     FILE* stream)
// {
//
//     if (
//
//
// }

// int compilerKey (Node_t* parent,
//                  CompilerContext_t* inf)
// {
//     assert (parent);
//     assert (inf);
//
//     if (parent->type != NODE_TYPE_KEY_WORD)
//         return 0;
//
//     switch (parent->value.key)
//     {
//         case (KEY_EXTENR_VAR):
//         {
//
//         }
//         case (KEY_EXTERN_FUNC):
//         {
//
//         }
//
//
//
//
//     }
//
// }

int compilerBlock (Node_t* parent,
                   CompilerContextInf_t* inf)
{
    assert (parent);
    assert (inf);

    NameTable_t new_table = {};
    nameTableCtr (&new_table);
    nameTableStackPush (inf->stack, &new_table);

    for (int i = 0; i < parent->amount_children; i++)
    {
        Node_t* node = parent->children[i];
        if (node->type == NODE_TYPE_KEY_WORD &&
            node->value.key == KEY_EXTERN_VAR)
        {
            if (node.)


        }
    }

}

int compilerExtVar (CompilerContextInf_t* inf,
                    Node_t* parent)
{
    assert (inf);
    assert (parent);

    if ()
}

NameTableVar_t* compilerFindVar (NameTableStack_t* stack,
                                 char* name)
{
    assert (stack);
    assert (name);

    NameTableStack_t new_stack = {};
    nameTableStackCtr (&new_stack);
    NameTableVar_t* var = NULL;

    for (size_t i = 0; i < nameTableStackGetSize (stack); i++)
    {
        NameTable_t* table = nameTableStackPop (stack);
        var = nameTableFind (table, name);

        nameTableStackPush (&new_stack, table);
        if (var == NULL)
            continue;
    }
    for (size_t i = 0; i < nameTableStackGetSize (&new_stack); i++)
        nameTableStackPush (stack, nameTableStackPop (&new_stack));

    nameTableStackDtr (&new_stack);
    return var;
}













