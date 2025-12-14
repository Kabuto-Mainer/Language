#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "NameTableFunc.h"
#include "SupportFunc.h"
#include "VectorFunc.h"
#include "TokenFunc.h"
#include "ParserFunc.h"
#include "DumpFunc.h"
#include "WriteTree.h"
#include "NodeFunc.h"
#include "ParseTree.h"
#include "CompilerFunc.h"

void compilerCode (const char* name_input,
                   const char* name_output);

void compilerTree (const char* name_tree,
                   const char* name_output);

void makeTree (const char* name_input,
               const char* name_tree);

int main (int argc, char* args[])
{
    if (argc == 1)
    {
        compilerCode ("myprog.txt", "code.asm");
    }

    else if (argc == 4)
    {
        const char* name_input = args[1];
        const char* name_output = args[3];
        const char* type = args[2];

        if (strcmp (type, "-ca") == 0)
        {
            compilerCode (name_input, name_output);
        }
        else if (strcmp (type, "-ct") == 0)
        {
            makeTree (name_input, name_output);
        }
        else if (strcmp (type, "-ta") == 0)
        {
            compilerTree (name_input, name_output);
        }
        else
        {
            printf ("Unknown flag\n");
        }
    }
    else
    {
        printf ("Must be 3 arguments\n");
    }

    return 0;
}

void compilerCode (const char* name_input,
                   const char* name_output)
{
    assert (name_input);
    assert (name_output);

    char* buffer = createCharBuffer (name_input);
    TokenVector_t vector = {};
    vectorCtr (&vector);
    tokenGlobal (buffer, &vector);
    // tokenDump (&vector);

    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->value.name = strdup ("main block");
    parserGlobal (vector.data, vector.size, root, name_input);

    treeDump (root, "text");
    // writeTree (root);
    compilerGlobal (name_output, root);

    vectorDtr (&vector);
    free (buffer);

    vectorDtrToken (root);
    free (root);
}

void compilerTree (const char* name_tree,
                   const char* name_output)
{
    assert (name_tree);
    assert (name_output);

    char* buffer = createCharBuffer ("Data/tree.txt");
    char* pose = buffer;
    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->value.name = strdup ("main block");

    readNode (root, &pose);
    treeDump (root, "text");

    root = root->children[0];
    free (root->parent);
    root->parent = NULL;

    treeDump (root, "tet");
    compilerGlobal (name_output, root);

    deleteNode (root);
    free (buffer);
}

void makeTree (const char* name_input,
               const char* name_tree)
{
    assert (name_input);
    assert (name_tree);

    char* buffer = createCharBuffer (name_input);
    TokenVector_t vector = {};
    vectorCtr (&vector);
    tokenGlobal (buffer, &vector);
    // tokenDump (&vector);

    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->value.name = strdup ("main block");
    parserGlobal (vector.data, vector.size, root, name_input);

    treeDump (root, "text");
    writeTree (root);

    vectorDtr (&vector);
    free (buffer);

    vectorDtrToken (root);
    free (root);
}

// void checkArgs (int argc, char* args)
// {
//     assert (args);
//
//     if (argc != 3)
//     {
//         printf ("Must be 0 or 2 arguments\n");
//         return;
//     }
//
//     const char* name_input = args[1];
//     const char* name_out
// }
