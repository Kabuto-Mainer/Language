#include <stdio.h>

#include "processor.h"

#include "../COMMON/comand.h"
#include "../COMMON/config.h"
#include "../COMMON/support.h"


int main(int argv, char* args[])
{

    printf("ARG: %d\n", argv);
    if (argv > 1) {
        for (int i = 1; i < argv; i++)
        {
            int amount_check_char = 0;
            if ((amount_check_char = check_flag(args[i], FLAGS[F_INP_FILE])) != -1)
            {
                // printf("NAME_INPUT_FILE: %s\n", &(args[i][2]));
                my_proc(&(args[i][amount_check_char]));
            }
        }
    }

    else {
        my_proc(NAME_BIN_FILE);
    }
    return 0;
}
