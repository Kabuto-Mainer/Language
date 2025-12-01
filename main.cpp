#include <stdlib.h>

#include "Support/SupportFunc.h"
#include "Vector/VectorFunc.h"
#include "Token/TokenFunc.h"


int main ()
{
    char* buffer = createCharBuffer ("test.txt");
    TokenVector_t vector = {};
    vectorCtr (&vector);

    tokenGlobal (buffer, &vector);
    tokenDump (&vector);

    vectorDtr (&vector);
    free (buffer);

    return 0;
}
