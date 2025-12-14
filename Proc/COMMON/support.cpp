#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "support.h"
#include "config.h"
#include "comand.h"
#include "color.h"

// -------------------------------------------------------------------------------------------------------
FILE* fopen_file(const char* name_file, const char* type_open)
{
    assert(name_file);

    // printf ("Name file: %s\n", name_file);
    FILE* buffer = fopen(name_file, type_open);
    asrt_to_file_f(buffer);

    return buffer;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
void fclose_file(FILE* address)
{
    assert(address);

    asrt_to_file( fclose(address) );
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
size_t file_size_check(const char* name_file)
{
    assert(name_file);

    struct stat file_stat = {}; //

    if (stat(name_file, &file_stat) == -1)
    {
        printf("ERROR with stat\n");
        return 0;
    }
//     Вроде как размер не должен быть отрицательным
    return (size_t) file_stat.st_size;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int* create_int_buffer(const size_t start_size)
{
    int* buffer_address = (int*) calloc(start_size, sizeof(int));

    if (buffer_address == NULL)
    {
        printf("ERROR with calloc in create_int_buffer\n");
        return NULL;
    }

    return buffer_address;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
char* create_char_buffer(const size_t start_size)
{
    char* buffer_address = (char*) calloc(start_size, sizeof(char));

    if (buffer_address == NULL)
    {
        printf("ERROR with calloc in create_int_buffer\n");
        return NULL;
    }

    return buffer_address;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int* realloc_buffer(int* address, const size_t max_size)
{
    int* buffer_address = (int*) realloc(address, max_size * MOD_REALLOC * sizeof(int));

    if (buffer_address == NULL)
    {
        printf("ERROR with realloc in realloc_buffer\n");
        return NULL;
    }

    return buffer_address;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
size_t pass_space(const char* string_place, size_t index)
{
    assert(string_place);

    while (string_place[index] == ' ')
    {
        index++;
    }
    return index;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
size_t find_char (const char* string, const char symbol)
{
    assert(string);

    for (size_t i = 0; ; i++)
    {
        if (*(string + i) == symbol || *(string + i) == '\n' || *(string + i) == '\0')
        {
            return i;
        }
    }
    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int check_inf(FILE* bin_file)
{
    assert(bin_file);

    int signature = 0;
    if (fread(&signature, sizeof(int), 1, bin_file) != 1)
    {
        fclose_file(bin_file);
        printf(_R_ "ERROR: read signature not correct\n" _N_);
        return -1;
    }

    if (signature != OWN_SIGNATURE)
    {
        printf(_R_ "ERROR: signature is not coincidence\n" _N_);
        return -1;
    }

    int version = 0;
    if (fread(&version, sizeof(int), 1, bin_file) != 1)
    {
        fclose_file(bin_file);
        printf(_R_ "ERROR: read version not correct\n" _N_);
        return -1;
    }

    if (version != CURRENT_VERSION)
    {
        printf(_R_ "ERROR: invalid version\n" _N_);
        return -1;
    }
    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int cmd_to_hash(const char* comand)
{
    assert(comand);

    int hash_cmd = 5183;

    for (int i = 0; i < 20; i++)
    {
        if (comand[i] == ' ' || comand[i] == '\0')
        {
            break;
        }

        hash_cmd += comand[i] * 33;
    }

    return hash_cmd;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int check_flag(const char* input_arg,
               const char* flag)
{
    assert(input_arg);
    assert(flag);

    size_t len_flag = strlen(flag);

    for (size_t i = 0; i < len_flag - 1; i++) {
        if (input_arg[i] != flag[i]) {
            return -1;
        }
    }

    return (int) len_flag;
}


