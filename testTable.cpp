#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "NameTableFunc.h"

int main ()
{
    NameTable_t table = {};
    nameTableCtr (&table);

    const char string[][20] = {
        "Var fdfdf1",
        "Var 2",
        "Var 3",
        "Var 4"
    };
    for (int i = 0; i < 4; i++)
    {
        char* name = strdup (string[i]);
        nameTableAdd (&table, name);

        NameTableVar_t* find_var = nameTableFind (&table, name);
        printf ("Name: %s\n", find_var->name);
        free (name);
    }
    // NameTableVar_t var = {};
    // var.name = strdup ("Test Var");
    // var.val.adr = 1000;
    // nameTableAdd (&table, var.name);
    // NameTableVar_t* find_var = nameTableFind (&table, var.name);
    // printf ("Name: %s\n", find_var->name);

    nameTableDtr (&table);
    return 0;
}



