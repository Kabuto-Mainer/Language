#ifndef ASM_TYPE_H
#define ASM_TYPE_H

#include "../COMMON/comand.h"

//------------------------------------------------------------------------------------------------
struct label_t
{
    int hash_label;
    char name[20];
    int address;
};
//------------------------------------------------------------------------------------------------
struct variable_t
{
    int hash_var;
    char name[20];
    int value;
};
//------------------------------------------------------------------------------------------------
struct asm_struct
{
    const char* name_file;
    FILE* text_stream;
    int* bin_code;
    char* asm_code;

    int sort_func[AMOUNT_CMD];

    label_t* table_label;
    int amount_labels;
    variable_t* table_var;
    int amount_vars;

    int amount_line;
    int cur_char;
    int cur_element;
    int hash_cmd;

    int current_run;
    int sound;
};
//------------------------------------------------------------------------------------------------
enum asm_error_t{
    A_NOT_ERRORS  = 0,
    FEW_PUSH      = 1,
    NO_ARG        = 2,
    UNKNOWN_REG   = 3,
    NO_ADR        = 4,
    NULL_ADR      = 5,
    INVALID_POINT = 6,
    FILE_ERROR    = 7,
    NO_REG        = 8,
    LIMIT         = 9,
    UNKNOWN_CMD   = 10,
    ERROR         = 11,
    INVALID_VAR   = 12,
    NO_VAR        = 13,
    NO_INT        = 14
};
//------------------------------------------------------------------------------------------------
const char DESCRIPTION_ERRORS[][40] =
{
    "no errors in code\n",
    "too few argument to push\n",
    "no register to use\n",
    "unknown register\n",
    "no address for jump\n",
    "null address in file\n",
    "not correct use point\n",
    "error with file\n",
    "no register to use\n",
    "too big or small argument\n",
    "unknown comand\n",
    "ERROR\n",
    "invalid variable\n",
    "no variable to use\n",
    "argument is not int\n"
};
//------------------------------------------------------------------------------------------------
typedef int (*compare_func)(const void*, const void* );
//------------------------------------------------------------------------------------------------





#endif




