

#include "../Common/NodeType.h"
#include "../Common/TypesOfType.h"
#include "../NameTable/NameTableFunc.h"

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

int compilerKey (Node_t* parent,
                 CompilerContext_t* inf)
{
    assert (parent);
    assert (inf);

    if (parent->type != NODE_TYPE_KEY_WORD)
        return 0;

    switch (parent->value.key)
    {
        case (KEY_EXTENR_VAR):
        {

        }
        case (KEY_EXTERN_FUNC):
        {

        }




    }

}

int compilerBlock (Node_t* parent,
                   CompilerContext_t* inf)
{
    assert (parent);
    assert (inf);

    NameTable_t new_table = {};
    nameTableCtr (&new_table);

    for (int i = 0; i < parent->amount_children; i++)
    {
        Node_t* node = parent->children[i];
        if (node->type == NODE_TYPE_KEY_WORD &&
            node->value.key == KEY_EXTERN_VAR)
        {



        }
    }

}

Memory_t compilerFindVar (NameTableStack_t* stack,
                          char* name)
{
    assert (stack);
    assert (name);

    NameTableStack_t new_stack = {};
    nameTableStackCtr (&new_stack);
    Memory_t address = 0;

    for (size_t i = 0; i < nameTableStackGetSize (stack); i++)
    {
        NameTable_t* table = nameTableStackPop (stack);
        size_t index = nameTableFind (table, name);
        if (index == (size_t) -1)
        {
            nameTableStackPush (new_stack, table);
            continue;
        }
        else
        {
            address =
        }

    }

}













