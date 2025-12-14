#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "assembler.h"
#include "asm-type.h"
#include "asm-sort.h"
#include "../COMMON/support.h"
#include "../COMMON/comand.h"
#include "../COMMON/config.h"


//------------------------------------------------------------------------------------------------
int check_arg(display_t* disp_set,
              audio_t* audio_set,
              const char** name_if,
              const char** name_of,
              int argv,
              char* args[])
{
    assert(disp_set);
    assert(audio_set);
    assert(name_if);
    assert(name_of);

//     if (argv != 2)
//     {
//         printf ("Must be 2 arguments in asm\n");
//         return 1;
//     }
//
//     *name_if = args[1];

    if (argv > 1)
    {
        for (int i = 1; i < argv; i++)
        {
            int len_flag = 0;

            if ((len_flag = check_flag(args[i], FLAGS[F_INP_FILE])) != -1)
            {
                *name_if = &(args[i][len_flag]);
                // printf("NAME_INPUT_FILE: %s\n", *name_if);
            }

            else if ((len_flag = check_flag(args[i], FLAGS[F_OUT_FILE])) != -1)
            {
                *name_of = &(args[i][len_flag]);
                // printf("NAME_INPUT_FILE: %s\n", *name_if);
            }

            else if ((len_flag = check_flag(args[i], FLAGS[F_SIZE_DISP])) != -1)
            {
                int buffer_len = -1;
                int buffer_high = -1;

                if (sscanf(args[i] + len_flag, "%d:%d", &buffer_len, &buffer_high) != 2 ||
                    buffer_len <= 0 || buffer_high <= 0)
                {
                    printf("ERROR: bad flag-size. Using standard settings\n");
                }

                else
                {
                    disp_set->len = buffer_len;
                    disp_set->high = buffer_high;
                    disp_set->size = buffer_len * buffer_high;
                    // printf("Use size: %d:%d\n", buffer_len, buffer_high);
                }
            }

            else if ((len_flag = check_flag(args[i], FLAGS[F_SOUND])) != -1)
            {
                int number_stream = 0;
                char name_input_file[200] = "";

                int buffer = 0;
                if ((buffer = sscanf(args[i] + len_flag, "%[^:]:%d", name_input_file, &number_stream)) != 2)
                {
                    printf("ERROR: bad flag-size. Not using sound\n");
                }
                else if (number_stream >= AMOUNT_SOUND_STREAM || number_stream < 0)
                {
                    printf("ERROR: bad sound_stream. Not using sound\n");
                }

                else
                {
                    audio_set->sound_stream = number_stream;
                    audio_set->code_stream = rand();
                    create_sound_stream(name_input_file,
                                        NAME_SOUND_STREAM[number_stream],
                                        audio_set->code_stream);
                    // printf("SOUND_STREAM: %d\n", number_stream);
                }
            }
        }
    }

    return 0;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int check_realloc(int** bin_code,
                  size_t* max_elements,
                  size_t current_element)
{
    assert(bin_code);
    assert(max_elements);

    if (current_element > *max_elements - REALLOC_CONST)
    {
        int* buffer_address = realloc_buffer(*bin_code, *max_elements);

        if (buffer_address == NULL)
        {
            printf(_R_ "ERROR in check_realloc: Null address\n" _N_);
            return -1;
        }
        *bin_code = buffer_address;

        *max_elements *= MOD_REALLOC;
    }
    return 0;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int char_reg_to_int(const char* name_reg)
{
    assert(name_reg);

    for (int i = 0; i < AMOUNT_REGISTERS; i++)
    {
        if (strcmp(name_reg, CHAR_REG[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
asm_error_t check_label(asm_struct* asm_data,
                  char* comand,
                  int add_index)
{
    assert(asm_data);

    if (asm_data->current_run != 0)
    {
        fprintf(asm_data->text_stream, "::: ");
        fprintf(asm_data->text_stream, " %s\n", asm_data->asm_code + asm_data->cur_char);

        asm_data->amount_line++;
        asm_data->cur_char += add_index + 1;

        return A_NOT_ERRORS;
    }

    int hash_label = cmd_to_hash(comand);

    for (int i = 0; i < AMOUNT_POINTS; i++)
    {
        if (asm_data->table_label[i].hash_label == 0)
        {
            asm_data->table_label[i].hash_label = hash_label;
            asm_data->table_label[i].address = asm_data->cur_element;
            asm_data->amount_labels++;

            strcpy(asm_data->table_label[i].name, comand);
            sort_label(asm_data);
            break;
        }

        if (asm_data->table_label[i].hash_label == hash_label)
        {
            if (strcmp(asm_data->table_label[i].name, comand) == 0){
                printf(_R_ "ERROR: redeclaration label\n" _N_);
            }

            else
            {
                printf(_R_ "ERROR: hash label has coincidenced, but name has not. Please, use another name\n");
            }
            EXIT_FUNCTION(asm_data, INVALID_POINT);
        }
    }

    asm_data->amount_line++;
    asm_data->cur_char += add_index + 1;

    fprintf(asm_data->text_stream, "::: ");
    return A_NOT_ERRORS;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
asm_error_t check_var(asm_struct* asm_data,
                      int add_index)
{
    char name[100] = "";
    int value = 0;
    sscanf(asm_data->asm_code + asm_data->cur_char, "%s %d", name, &value);

    int hash_var = cmd_to_hash(name);
    // printf("NAME: %s\n", name);

    for (int i = 0; i < AMOUNT_VARS; i++)
    {
        if (hash_var == asm_data->table_var[i].hash_var)
        {
            if (strcmp(asm_data->table_var[i].name, name) == 0)
            {
                printf("ERROR: redeclaration const num \"%s\n\"", name);
            }

            else
            {
                printf(_R_ "ERROR: hash var has coincidenced, but name has not. Please, use another name\n");
            }
            EXIT_FUNCTION(asm_data, INVALID_VAR);
        }

        else if (asm_data->table_var[i].hash_var == 0)
        {
            asm_data->table_var[i].hash_var = hash_var;
            asm_data->table_var[i].value = value;
            asm_data->amount_vars++;

            strcpy(asm_data->table_var[i].name, name);
            sort_var(asm_data);
            break;
        }

        if (i == AMOUNT_VARS - 1)
        {
            printf("ERROR: too many variables\n");
            EXIT_FUNCTION(asm_data, INVALID_VAR);
        }
    }

    asm_data->amount_line++;
    asm_data->cur_char += add_index + 1;

    fprintf(asm_data->text_stream, "### ");
    return A_NOT_ERRORS;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int skip_trash(asm_struct* asm_data,
               char* comand,
               int add_index)
{
    assert(asm_data);

    if (comand[0] == ';' || comand[0] == '\0') {
        asm_data->amount_line++;
        asm_data->cur_char += add_index + 1;
        return 0;
    }
    return 1;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
asm_error_t my_assembler (const char* name_asm_file,
                          const char* name_byte_file,
                          const char* name_text_file,
                          label_t* table_label,
                          int* amount_labels,
                          int number_start,
                          display_t* disp_set,
                          audio_t* audio_set)
{
    assert(name_asm_file);
    assert(name_byte_file);
    assert(name_text_file);
    assert(table_label);
    assert(disp_set);
    assert(audio_set);

    asm_struct asm_data = {};
    asm_data.current_run = number_start;
    asm_data.amount_vars = 0;
    asm_data.amount_labels = *amount_labels;
    variable_t table_var[AMOUNT_VARS] = {};


    int amount_chars = asm_create(
                                &asm_data,
                                name_asm_file,
                                name_text_file,
                                table_label,
                                table_var
    );

    size_t max_elements = START_AMOUNT_CMD;
    fprintf(asm_data.text_stream, "[ADR] | COMAND  ARGUMENT | ASM CODE\n");

    while (asm_data.cur_char < amount_chars)
    {
        if (check_realloc(&(asm_data.bin_code), &max_elements, (size_t) asm_data.cur_element) == -1)
        {
            free(asm_data.bin_code);
            free(asm_data.asm_code);
            fclose_file(asm_data.text_stream);
            return NULL_ADR;
        }

        int add_index = (int) find_char(asm_data.asm_code + asm_data.cur_char, '\n');
        *(asm_data.asm_code + asm_data.cur_char + add_index) = '\0';
        // printf("STR: -%s-\n", asm_data.asm_code + asm_data.cur_char);

        char comand[200] = "";
        sscanf(asm_data.asm_code + asm_data.cur_char, "%39s", comand);

        // printf("CMD: -%s-\n", comand);

        if (skip_trash(&asm_data, comand, add_index) == 0)
        {
            continue;
        }

        // printf("CMD: -%s-\n", comand);

        if (comand[0] == ':')
        {
            if (check_label(&asm_data, comand, add_index) == A_NOT_ERRORS)
            {
                continue;
            }

            else
            {
                printf(_R_ "ERROR compilation end with error\n" _N_);
                free(asm_data.bin_code);
                free(asm_data.asm_code);
                return ERROR;
            }
        }

        else if (comand[0] == '#')
        {
            if (check_var(&asm_data, add_index) == A_NOT_ERRORS) {
                // asm_data.amount_line++;
                // asm_data.cur_char += add_index + 1;
                continue;
            }

            else
            {
                printf(_R_ "ERROR compilation end with error\n" _N_);
                free(asm_data.bin_code);
                free(asm_data.asm_code);
                return ERROR;
            }
        }

        else
        {
            fprintf(asm_data.text_stream, "[%3d] |", asm_data.cur_element - AMOUNT_SUP_NUM + 1);

            asm_data.hash_cmd = cmd_to_hash(comand);

            if (asm_check_cmd(&asm_data) != A_NOT_ERRORS) {
                free(asm_data.bin_code);
                free(asm_data.asm_code);
                printf(_R_ "ERROR compilation end with error\n" _N_);
                return ERROR;
            }
        }
        fprintf(asm_data.text_stream, " %s\n", asm_data.asm_code + asm_data.cur_char);

        asm_data.cur_char += add_index + 1;
        asm_data.amount_line++;

    }

    asm_data.bin_code[0] = OWN_SIGNATURE;
    asm_data.bin_code[1] = CURRENT_VERSION;
    asm_data.bin_code[2] = asm_data.cur_element;
    asm_data.bin_code[3] = disp_set->len;
    asm_data.bin_code[4] = disp_set->high;
    asm_data.bin_code[5] = audio_set->sound_stream;
    asm_data.bin_code[6] = audio_set->code_stream;

    *amount_labels = asm_data.amount_labels;

    FILE* bin_file = fopen_file(name_byte_file, "wb");
    if (fwrite(asm_data.bin_code, sizeof(int), (size_t) asm_data.cur_element, bin_file) != (size_t) asm_data.cur_element)
    {
        printf(_R_ "ERROR: write bin_code to bin_file was failed\n" _N_);
        free(asm_data.bin_code);
        free(asm_data.asm_code);
        return FILE_ERROR;
    }
    fclose_file(bin_file);
    fclose_file(asm_data.text_stream);

    free(asm_data.bin_code);
    free(asm_data.asm_code);
    printf(_G_ "Compilation was completed\n" _N_);

    return A_NOT_ERRORS;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
asm_error_t asm_check_cmd(asm_struct* asm_data)
{
    assert(asm_data);

    int index = search_func(asm_data->sort_func, asm_data->hash_cmd);
    // for (size_t i = 0; i < AMOUNT_CMD; i++)
    // {
    //     // printf("I = %zu\n", i);
    //     if (asm_data->hash_cmd == CMD_INF[i].hash_cmd)
    //     {
    if (index == -1)
    {
        EXIT_FUNCTION(asm_data, UNKNOWN_CMD);
    }

    if ((CMD_INF[index]).amount_arg == 0)
    {
        asm_data->bin_code[asm_data->cur_element++] = CMD_INF[index].number;

        fprintf(asm_data->text_stream, "%08d          |",
                CMD_INF[index].number);

        return A_NOT_ERRORS;
    }
//------------------------------------------------------------------------------------------------
    else if (CMD_INF[index].type_arg == NUM)
    {
        if (check_NUM_cmd(asm_data, (size_t) index) != A_NOT_ERRORS)
        {
            return ERROR;
        }

        return A_NOT_ERRORS;
    }
//------------------------------------------------------------------------------------------------
    else if ((CMD_INF[index]).type_arg == ADR)
    {
        if (check_jmp_cmd(asm_data, (CMD_INF[index]).number) != A_NOT_ERRORS)
        {
            return ERROR;
        }

        return A_NOT_ERRORS;
    }
//------------------------------------------------------------------------------------------------
    else if ((CMD_INF[index]).type_arg == REG)
    {
        if (check_REG_cmd(asm_data, (size_t) index) != A_NOT_ERRORS)
        {
            return ERROR;
        }

        return A_NOT_ERRORS;
    }

    return ERROR;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
asm_error_t check_jmp_cmd(asm_struct* asm_data, int cmd)
{
    assert(asm_data);
    assert(asm_data->table_label);

    char char_adr[41] = "";
    char trash[100] = "";
    int new_C_E = 0;


    if (sscanf(asm_data->asm_code + asm_data->cur_char + 1, "%s %s", trash, char_adr) == 1)
    {
        EXIT_FUNCTION(asm_data, NO_ADR);
    }

    char_adr[40] = '\0'; // На всякий

    if (char_adr[0] == ':')
    {
        int hash_label = cmd_to_hash(char_adr);
        label_t* label = search_label(asm_data, hash_label);

        if (label == NULL)
        {
            EXIT_FUNCTION(asm_data, INVALID_POINT);
        }

        new_C_E = (*label).address - AMOUNT_SUP_NUM + 1;
    }

    else if (char_adr[0] == '$') {
        int add_num = atoi(char_adr + 1);
        if (add_num < 0) {
            EXIT_FUNCTION(asm_data, INVALID_POINT);
        }

        new_C_E = asm_data->cur_element + add_num - 3;
    }

    else if (isdigit(char_adr[0]) != 0) {
        new_C_E = atoi(char_adr);
    }

    else {
        EXIT_FUNCTION(asm_data, INVALID_POINT);
    }

    asm_data->bin_code[asm_data->cur_element++] = cmd;
    asm_data->bin_code[asm_data->cur_element++] = new_C_E;

    fprintf(asm_data->text_stream, "%08d %08d |", cmd, (int) new_C_E);
    return A_NOT_ERRORS;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
asm_error_t check_NUM_cmd(asm_struct* asm_data,
                          size_t cur_el_struct)
{
    assert(asm_data);

    int add_index = 0;
    char arg_c[100] = "";
    char trash[100] = "";
    int arg = 0;
//
//     if (asm_data->hash_cmd == HASH_SOUND ||

    sscanf(asm_data->asm_code + asm_data->cur_char, "%s %n", trash, &add_index);

    asm_data->bin_code[asm_data->cur_element++] = CMD_INF[cur_el_struct].number;

    fprintf(asm_data->text_stream, "%08d ",
            CMD_INF[cur_el_struct].number);

    for (int i = 0; i < CMD_INF[cur_el_struct].amount_arg; i++)
    {
        int buffer = 0;
        // printf("I = %d\n", i);
        if (sscanf(asm_data->asm_code + asm_data->cur_char + add_index,
                   "%s %n", arg_c, &buffer) != 1)
        {
            EXIT_FUNCTION(asm_data, NO_ARG);
        }

        if (arg_c[0] == '#')
        {
            int hash_var = cmd_to_hash(arg_c);
            variable_t* var = search_var(asm_data, hash_var);

            if (var == NULL)
            {
                EXIT_FUNCTION(asm_data, NO_VAR);
            }

            arg = var->value;
        }

        else
        {
            arg = atoi(arg_c);

            if (arg == 0 && arg_c[0] != '0')
            {
                EXIT_FUNCTION(asm_data, NO_INT);
            }
        }

        if (arg < MIN_MEAN || arg > MAX_MEAN)
        {
            EXIT_FUNCTION(asm_data, LIMIT);
        }
        asm_data->bin_code[asm_data->cur_element++] = arg;
        add_index += buffer;

        fprintf(asm_data->text_stream, "%08d ", arg);
    }

    fprintf(asm_data->text_stream, "|");

    return A_NOT_ERRORS;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
asm_error_t check_REG_cmd(asm_struct* asm_data,
                          size_t cur_el_struct)
{
    assert(asm_data);

    char char_reg[4] = "";
    char trash[100] = "";

    if (sscanf(asm_data->asm_code + asm_data->cur_char, "%s %s", trash, char_reg) == 1)
    {
        EXIT_FUNCTION(asm_data, NO_REG);
    }

    int reg = 0;
    if ((reg = char_reg_to_int(char_reg)) == -1)
    {
        EXIT_FUNCTION(asm_data, UNKNOWN_REG);
    }

    asm_data->bin_code[asm_data->cur_element++] = (CMD_INF[cur_el_struct]).number;
    asm_data->bin_code[asm_data->cur_element++] = reg;

    fprintf(asm_data->text_stream, "%08d %08d |", (CMD_INF[cur_el_struct]).number, reg);

    return A_NOT_ERRORS;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int sign_to_j_cmd(const char* string)
{
    assert(string);

    if (string[0] == '<') {
        if (string[1] == '=') {
            return INT_JBE;
        }

        else {
            return INT_JB;
        }
    }

    if (string[0] == '>') {
        if (string[1] == '=') {
            return INT_JAE;
        }

        else {
            return INT_JA;
        }
    }

    if (string[1] == '=') {
        if (string[0] == '=') {
            return INT_JE;
        }

        else if (string[0] == '!') {
            return INT_JNE;
        }

        else {
            return -1;
        }
    }

    return -1;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int asm_create(asm_struct* asm_data,
               const char* name_asm_file,
               const char* name_text_file,
               label_t* table_label,
               variable_t* table_var)
{

    asm_data->name_file = name_asm_file;
    asm_data->table_label = table_label;
    asm_data->amount_line = 0;
    asm_data->cur_char = 0;
    asm_data->cur_element = AMOUNT_SUP_NUM;

    sort_func(asm_data);

    size_t max_elements = START_AMOUNT_CMD;

    FILE* asm_stream = fopen_file(name_asm_file, "rb");
    FILE* text_stream = fopen_file(name_text_file, "w");

    char* asm_code = create_char_buffer(file_size_check(name_asm_file) + 1);
    if (asm_code == NULL) {
        return -1;
    }

    int amount_chars = (int) fread(asm_code, sizeof(char), file_size_check(name_asm_file), asm_stream);
    asm_code[amount_chars] = '\0';
    fclose_file(asm_stream);

    for (int i = 0; i < AMOUNT_VARS; i++) {
        table_var[i] = {0, 0};
    }
    asm_data->table_var = &(table_var[0]);

    int* bin_code = create_int_buffer(max_elements);

    asm_data->bin_code = bin_code;
    asm_data->asm_code = asm_code;
    asm_data->text_stream = text_stream;

    return amount_chars;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int bin_find(const int hash_cmd)
{
    int cur_element = AMOUNT_CMD / 2;
    while (cur_element != 1)
    {
        if (hash_cmd < CMD_INF[cur_element].hash_cmd)
        {
            cur_element += cur_element / 2;
        }

        else if (hash_cmd == CMD_INF[cur_element].hash_cmd)
        {
            return cur_element;
        }

        else
        {
            cur_element -= cur_element / 2;
        }
    }

    return -1;
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
int create_sound_stream(const char* name_sound_file,
                        const char* name_sound_stream,
                        const int code_stream)
{
    assert(name_sound_file);
    assert(name_sound_stream);

    FILE* input_stream = fopen_file(name_sound_file, "rb");
    FILE* output_stream = fopen_file(name_sound_stream, "wb");

    size_t size_input = file_size_check(name_sound_file);

    int* buffer = (int*) calloc(size_input, sizeof(int));

    size_t amount_check_el = fread(buffer, sizeof(int), size_input, input_stream);
    fwrite(&code_stream, sizeof(int), 1, output_stream);
    fwrite(buffer, sizeof(int), amount_check_el, output_stream);


    fclose_file(input_stream);
    fclose_file(output_stream);
    free(buffer);

    return 0;
}
//------------------------------------------------------------------------------------------------
//
// //------------------------------------------------------------------------------------------------
// // TODO +-
// int compare_label(const void* struct_1, const void* struct_2)
// {
//     const label_t* label_1 = (const label_t*) struct_1;
//     const label_t* label_2 = (const label_t*) struct_2;
//
//     return (label_1->hash_label - label_2->hash_label);
// }
// //------------------------------------------------------------------------------------------------
//
// //------------------------------------------------------------------------------------------------
// int compare_var(const void* struct_1, const void* struct_2)
// {
//     const variable_t* var_1 = (const variable_t*) struct_1;
//     const variable_t* var_2 = (const variable_t*) struct_2;
//
//     return (var_1->hash_var - var_2->hash_var);
// }
// //------------------------------------------------------------------------------------------------
//
// //------------------------------------------------------------------------------------------------
// int compare_asm_func(const void* struct_1, const void* struct_2)
// {
//     const asm_func_t* func_1 = (const asm_func_t*) struct_1;
//     const asm_func_t* func_2 = (const asm_func_t*) struct_2;
//
//     return (func_1->hash_cmd - func_2->hash_cmd);
// }
// //------------------------------------------------------------------------------------------------
//
// //------------------------------------------------------------------------------------------------
// int sort_label(asm_struct* asm_data)
// {
//     assert(asm_data);
//
//     qsort(asm_data->table_label,
//           (size_t) asm_data->amount_labels,
//           sizeof(asm_data->table_label[0]),
//           &compare_label);
//
//     return 0;
// }
// //------------------------------------------------------------------------------------------------
//
// //------------------------------------------------------------------------------------------------
// int sort_var(asm_struct* asm_data)
// {
//     assert(asm_data);
//
//     qsort(asm_data->table_var,
//           (size_t) asm_data->amount_vars,
//           sizeof(asm_data->table_var[0]),
//           &compare_var);
//
//     return 0;
// }
// //------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
// int find_label(asm_struct* asm_data)
// {
//     assert(asm_data);
//
//     int* index_hash = (int*) bsearch(
//     asm_data->hash_cmd
//
//
// }



