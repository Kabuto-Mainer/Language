#include <stdlib.h>

#include "NameTableFunc.h"
#include "SupportFunc.h"
#include "VectorFunc.h"
#include "TokenFunc.h"
#include "ParserFunc.h"

int main ()
{
    char* buffer = createCharBuffer ("test.txt");
    TokenVector_t vector = {};
    vectorCtr (&vector);

    tokenGlobal (buffer, &vector);
    tokenDump (&vector);

    Node_t* root = newNode ();
    parserGlobal (vector.  root, )

    vectorDtr (&vector);
    free (buffer);

    return 0;
}
