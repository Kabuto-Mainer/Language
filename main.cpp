#include <stdlib.h>

#include "NameTable/NameTableFunc.h"
#include "Support/SupportFunc.h"
#include "Vector/VectorFunc.h"
#include "Token/TokenFunc.h"


int main ()
{
    char* buffer = createCharBuffer ("test.txt");
    TokenVector_t vector = {};
    vectorCtr (&vector);

    NameTable_t table_func = {};
    nameTableCtr (&table_func);

    tokenGlobal (buffer, &vector, &table_func);
    tokenDump (&vector);

    vectorDtr (&vector);
    free (buffer);

    return 0;
}
