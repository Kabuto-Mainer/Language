#include <stdlib.h>
#include <string.h>

#include "NameTableFunc.h"
#include "SupportFunc.h"
#include "VectorFunc.h"
#include "TokenFunc.h"
#include "ParserFunc.h"
#include "DumpFunc.h"
#include "WriteTree.h"
#include "NodeFunc.h"


int main ()
{
    char* buffer = createCharBuffer ("Data/test.txt");
    TokenVector_t vector = {};
    vectorCtr (&vector);

    tokenGlobal (buffer, &vector);
    // tokenDump (&vector);

    Node_t* root = newNode ();
    root->type = NODE_TYPE_BLOCK;
    root->value.name = strdup ("main block");
    parserGlobal (vector.data, vector.size, root,"test.txt");
    // dumpNode (*((*(root->children))->children));
    treeDump (root, "text");
    writeTree (root);

    vectorDtr (&vector);
    free (buffer);

    vectorDtrToken (root);
    free (root);

    return 0;
}
