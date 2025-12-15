#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*
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
#include "WriteCode.h"


// ---------------------------------------------------------------------------------------------------
void codeToAsm (const char* name_input,
                const char* name_output);
// ---------------------------------------------------------------------------------------------------
void astToAsm (const char* name_tree,
               const char* name_output);
// ---------------------------------------------------------------------------------------------------
void codeToAst (const char* name_input,
                const char* name_tree);
// ---------------------------------------------------------------------------------------------------
void astToCode (const char* name_input,
                const char* name_output);
// ---------------------------------------------------------------------------------------------------
void codeToCode (const char* name_input,
                 const char* name_output);
// ---------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------
int main (int argc, char* args[])
{
    if (argc == 1)
    {
        codeToAsm ("myprog.txt", "code.asm");
    }

    else if (argc == 4)
    {
        const char* name_input = args[1];
        const char* name_output = args[3];
        const char* type = args[2];

        // printf ("INPUT: %s\nPUTPUT: %s\n", name_input, name_output);
        if (strcmp (type, "-ca") == 0)
        {
            codeToAsm (name_input, name_output);
        }
        else if (strcmp (type, "-ct") == 0)
        {
            codeToAst (name_input, name_output);
        }
        else if (strcmp (type, "-ta") == 0)
        {
            astToAsm (name_input, name_output);
        }
        else if (strcmp (type, "-tc") == 0)
        {
            astToCode (name_input, name_output);
        }
        else if (strcmp (type, "-cc") == 0)
        {
            codeToCode (name_input, name_output);
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
// ---------------------------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------------------------
void codeToAsm (const char* name_input,
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

    treeDump (root, "Read Code and covert to Tree before covert to Assembler");
    // writeTree (root);
    compilerGlobal (name_output, root);

    vectorDtr (&vector);
    free (buffer);

    vectorDtrToken (root);
    free (root);
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
void astToAsm (const char* name_tree,
               const char* name_output)
{
    assert (name_tree);
    assert (name_output);

    char* buffer = createCharBuffer (name_tree);
    char* pose = buffer;
    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->value.name = strdup ("main block");

    readNode (root, &pose);
    root = root->children[0];
    free (root->parent);
    root->parent = NULL;

    treeDump (root, "Read AST-tree before convert to Assembler");
    compilerGlobal (name_output, root);

    deleteNode (root);
    free (buffer);
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
void codeToAst (const char* name_input,
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

    treeDump (root, "Read Code and covert to Tree before write Tree");
    writeTree (root);

    vectorDtr (&vector);
    free (buffer);

    vectorDtrToken (root);
    free (root);
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
void astToCode (const char* name_input,
                const char* name_output)
{
    assert (name_input);
    assert (name_output);

    char* buffer = createCharBuffer (name_input);
    char* pose = buffer;
    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->value.name = strdup ("main block");

    readNode (root, &pose);
    root = root->children[0];
    free (root->parent);
    root->parent = NULL;

    treeDump (root, "Read AST-tree before convert to Code");
    writeCodeToFile (root, name_output);

    deleteNode (root);
    free (buffer);

    return;
}
// ---------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------
void codeToCode (const char* name_input,
                 const char* name_output)
{
    assert (name_input);
    assert (name_output);

    char* buffer = createCharBuffer (name_input);
    TokenVector_t vector = {};
    vectorCtr (&vector);
    tokenGlobal (buffer, &vector);

    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->value.name = strdup ("main block");
    parserGlobal (vector.data, vector.size, root, name_input);

    treeDump (root, "Read AST-tree before convert to Code");
    writeCodeToFile (root, name_output);

    vectorDtr (&vector);
    free (buffer);

    vectorDtrToken (root);
    free (root);

    return;
}
// ---------------------------------------------------------------------------------------------------
*/

// #include "NameTableFunc.h"
#include "SupportFunc.h"
#include "VectorFunc.h"
#include "TokenFunc.h"
#include "ParserFunc.h"
#include "DumpFunc.h"
// #include "WriteTree.h"
#include "NodeFunc.h"
// #include "ParseTree.h"
// #include "CompilerFunc.h"
// #include "WriteCode.h"

int main ()
{
    char* buffer = createCharBuffer ("prog.txt");
    TokenVector_t vector = {};
    vectorCtr (&vector);
    tokenGlobal (buffer, &vector);
    tokenDump (&vector);

    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->val.name = strdup ("main block");
    parserGlobal (vector.data, vector.size, root, "prog.txt");
    treeDump (root, "Read AST-tree before convert to Code");

    return 0;
}
