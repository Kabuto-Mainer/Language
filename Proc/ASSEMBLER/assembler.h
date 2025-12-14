#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include "asm-type.h"
#include "../COMMON/config.h"
#include "../COMMON/color.h"
#include "../COMMON/comand.h"
#include "../COMMON/cmd-hash.h"
#include "../PROCESSOR/stack.h"
#include "../PROCESSOR/stack_define.h"

//------------------------------------------------------------------------------------------------
#define EXIT_FUNCTION(data, return_error) \
 printf(_R_ "%s:%d: ERROR: %s\n" _N_, data->name_file, data->amount_line + 1, DESCRIPTION_ERRORS[return_error]); \
 return return_error;
//------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
asm_error_t my_assembler (const char* name_asm_file,
                          const char* name_byte_file,
                          const char* name_text_file,
                          label_t* table_label,
                          int* amount_labels,
                          int number_start,
                          display_t* disp_set,
                          audio_t* audio_set);
//------------------------------------------------------------------------------------------------
int asm_create(asm_struct* asm_data,
               const char* name_asm_file,
               const char* name_text_file,
               label_t* table_label,
               variable_t* table_var);
//------------------------------------------------------------------------------------------------
int check_arg(display_t* disp_set,
              audio_t* audio_set,
              const char** name_if,
              const char** name_of,
              int argv,
              char* args[]);
//------------------------------------------------------------------------------------------------
asm_error_t check_jmp_cmd(asm_struct* asm_data,
                        int cmd);
//------------------------------------------------------------------------------------------------
asm_error_t check_NUM_cmd(asm_struct* asm_data,
                          size_t cur_el_struct);
//------------------------------------------------------------------------------------------------
asm_error_t check_REG_cmd(asm_struct* asm_data,
                          size_t cur_el_struct);
//------------------------------------------------------------------------------------------------
asm_error_t asm_check_cmd(asm_struct* asm_data);
//------------------------------------------------------------------------------------------------
int skip_trash(asm_struct* asm_data,
                     char* comand,
                     int add_index);
//------------------------------------------------------------------------------------------------
asm_error_t check_label(asm_struct* asm_data,
                  char* comand,
                  int add_index);
//------------------------------------------------------------------------------------------------
asm_error_t check_var(asm_struct* asm_data,
              int add_index);
//------------------------------------------------------------------------------------------------
int check_realloc(int** bin_code,
                  size_t* max_size,
                  size_t current_size);
//------------------------------------------------------------------------------------------------
int char_reg_to_int(const char* name_reg);
//------------------------------------------------------------------------------------------------
int sign_to_j_cmd(const char* string);
//------------------------------------------------------------------------------------------------
int bin_find(const int hash_cmd);
//------------------------------------------------------------------------------------------------
int bubble_sort(const int* mass_num, const int amount_el);
//------------------------------------------------------------------------------------------------
int create_sound_stream(const char* name_sound_file,
                        const char* name_sound_stream,
                        const int code_stream);
//------------------------------------------------------------------------------------------------
// int compare_label(const void* struct_1, const void* struct_2);
// //------------------------------------------------------------------------------------------------
// int compare_var(const void* struct_1, const void* struct_2);
// //------------------------------------------------------------------------------------------------
// int compare_asm_func(const void* struct_1, const void* struct_2);
// //------------------------------------------------------------------------------------------------
// int sort_label(asm_struct* asm_data);
// //------------------------------------------------------------------------------------------------
// int sort_var(asm_struct* asm_data);
// //------------------------------------------------------------------------------------------------





#endif // DISASSEMBLER_H
