#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "Compiler.h"
#include "Support/SupportFunc.h"


int compilerCreateTree (CompilerTree_t* tree)
{
    assert (tree);

    CompilerNode_t* root = newNode ();

    root->amount_children = 0;
    root->children = NULL;
    root->parent = NULL;
    root->type = NODE_TYPE_GLOBAL;
    root->value.index = 0;

    tree->size = 0;
    return 0;
}

int compilerDestructTree (CompilerTree_t* tree)
{
    assert (tree);

    compilerDeleteNode (tree->root);
    return 0;
}

int compilerDeleteNode (CompilerNode_t* node)
{
    assert (node);

    for (size_t i = 0; i < node->amount_children; i++)
        compilerDeleteNode (node->children[i]);

    free (node);
    return 0;
}

CompilerNode_t* newNode ()
{
    CompilerNode_t* node = (CompilerNode_t*) calloc (1, sizeof (CompilerNode_t));
    if (node == NULL)
        EXIT_FUNC("NULL calloc", NULL);
    return node;
}



