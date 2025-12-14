#include <stdio.h>
#include <assert.h>
#include <cstdlib>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "stack_define.h"
#include "proc-type.h"
#include "display.h"
#include "proc-func.h"
#include "processor.h"
#include "stack.h"
#include "../COMMON/support.h"
#include "../COMMON/comand.h"
#include "../COMMON/color.h"

// -------------------------------------------------------------------------------------------------------
int* create_bin_buffer(const char* name_bin_file,
                       int* amount_elements,
                       display_t* disp_set,
                       audio_t* audio_set) {
    assert(name_bin_file);
    assert(disp_set);
    assert(audio_set);
    assert(amount_elements);

    FILE* bin_file = fopen_file(name_bin_file, "rb");

    if (check_inf(bin_file) != 0)
    {
        return NULL;
    }

    if (fread(amount_elements, sizeof(int), 1, bin_file) != 1)
    {
        fclose_file(bin_file);
        printf(_R_ "ERROR: read amount_elements not correct\n" _N_);
        return NULL;
    }

    if (fread(&(disp_set->len), sizeof(int), 1, bin_file) != 1)
    {
        fclose_file(bin_file);
        printf(_R_ "ERROR: read LEN_DISPLAY not correct\n" _N_);
        return NULL;
    }

    if (fread(&(disp_set->high), sizeof(int), 1, bin_file) != 1)
    {
        fclose_file(bin_file);
        printf(_R_ "ERROR: read HIGH_DISPLAY not correct\n" _N_);
        return NULL;
    }
    disp_set->size = disp_set->len * disp_set->high;

    if (fread(&(audio_set->sound_stream), sizeof(int), 1, bin_file) != 1)
    {
        fclose_file(bin_file);
        printf(_R_ "ERROR: read NUMBER_STREAM not correct\n" _N_);
        return NULL;
    }

    if (fread(&(audio_set->code_stream), sizeof(int), 1, bin_file) != 1)
    {
        fclose_file(bin_file);
        printf(_R_ "ERROR: read CODE_STREAM not correct\n" _N_);
        return NULL;
    }

    printf("LEN: %d\nHIGH: %d\n", disp_set->len, disp_set->high);

//! Нам точно известно, что amount_elements > 2
    int* bin_code = create_int_buffer(size_t (*amount_elements - AMOUNT_SUP_NUM));
    if (fread(bin_code, sizeof(int), size_t(*amount_elements - AMOUNT_SUP_NUM), bin_file)
        != size_t(*amount_elements - AMOUNT_SUP_NUM))
    {

        free(bin_code);
        fclose_file(bin_file);
        printf(_R_ "ERROR: with read bin_code\n" _N_);
        return NULL;
    }

    fclose_file(bin_file);
    return bin_code;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int my_proc(const char* name_bin_file)
{
    assert(name_bin_file);

    if (check_correct_func() != P_OK)
    {
        printf(_R_ "ERROR: not correct CMD_INF\n" _N_);
        return -1;
    }

    int amount_elements = 0;
    cpu_t proc = {};
    display_t disp_set = {LEN_DISPLAY, HIGH_DISPLAY, VRAM_SIZE};
    proc.disp_set = disp_set;

    if (cpu_ctor(&proc, name_bin_file, &amount_elements) == -1) {
        printf(_R_ "ERROR: cpu_ctor return -1\n" _N_);
        return -1;
    }

    for ( ; size_t (proc.C_E) < size_t (amount_elements - AMOUNT_SUP_NUM); (proc.C_E)++) {
        // if (getchar() == 'd') {
            // proc_dump(&proc);
//         // }
//         switch (proc.bin_code[proc.C_E])
//         {
//             case INT_PUSH:
//             {
//                 proc.C_E++;
//
//                 stack_push(&(proc.stack), proc.bin_code[proc.C_E]);
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_IN:
//             {
//                 int num = 0;
//
//                 printf(_G_ "Enter int num: " _N_);
//                 scanf("%d", &num);
//
//                 stack_push(&(proc.stack), num);
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_ADD:
//             {
//                 ARF_COMAND(+);
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_SUB:
//             {
//                 ARF_COMAND(-);
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_DIV:
//             {
//                 int arg_1 = 0;
//                 int arg_2 = 0;
//
//                 stack_pop(&(proc.stack), &arg_1);
//                 stack_pop(&(proc.stack), &arg_2);
//
//                 if (arg_2 == 0) {
//                     printf(_R_ "ERROR: division by zero is not correct\n" _N_);
//                     cpu_dtor(&proc);
//                     return -1;
//                 }
//
//                 stack_push(&(proc.stack), arg_1 / arg_2);
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_MUL:
//             {
//                 ARF_COMAND(*);
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_SQRT:
//             {
//                 int arg = 0;
//                 stack_pop(&(proc.stack), &arg);
//
//                 if (arg < 0)
//                 {
//                     printf(_R_ "ERROR: sqrt in negative num is not correct\n" _N_);
//                     cpu_dtor(&proc);
//                     return -1;
//                 }
//                 stack_push(&(proc.stack), (int) sqrt(arg));
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_OUT:
//             {
//                 int arg = 0;
//
//                 stack_pop(&(proc.stack), &arg);
//                 printf("%d\n", arg);
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_HLT:
//             {
//                 cpu_dtor(&proc);
//                 return 0;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_POPR:
//             {
//                 int reg = proc.bin_code[++proc.C_E];
//                 int num = 0;
//
//                 stack_pop(&(proc.stack), &num);
//                 proc.regs[reg] = num;
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_PUSHR:
//             {
//                 int reg = proc.bin_code[++proc.C_E];
//                 int num = proc.regs[reg];
//
//                 stack_push(&(proc.stack), num);
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_JMP:
//             {
//                 proc.C_E++;
//                 proc.C_E = proc.bin_code[proc.C_E] - 2;
//
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_HACK:
//             {
//                 proc.stack.size = -1;
//                 break;
//             }
// //------------------------------------------------------------------------------------------------
//             case INT_JB:
//             {
//                 J_COMAND(<);
//             }
// //---------------------------------------------------------------------------------------------------
//             case INT_JBE:
//             {
//                 J_COMAND(<=);
//             }
// //---------------------------------------------------------------------------------------------------
//             case INT_JA:
//             {
//                 J_COMAND(>);
//             }
// //---------------------------------------------------------------------------------------------------
//             case INT_JAE:
//             {
//                 J_COMAND(>=);
//             }
// //---------------------------------------------------------------------------------------------------
//             case INT_JE:
//             {
//                 J_COMAND(==);
//             }
// //---------------------------------------------------------------------------------------------------
//             case INT_JNE:
//             {
//                 J_COMAND(!=);
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_CALL:
//             {
//                 proc.C_E++;
//                 stack_push(&(proc.address), proc.C_E + 1);
//                 proc.C_E = proc.bin_code[proc.C_E] - 2;
//
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_RET:
//             {
//                 int adr = 0;
//                 stack_pop(&(proc.address), &adr);
//
//                 proc.C_E = adr - 1;
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_PUSHM:
//             {
//                 int arg = proc.RAM[proc.regs[proc.bin_code[++(proc.C_E)]]];
//
//                 stack_push(&(proc.stack), arg);
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_POPM:
//             {
//                 int arg = 0;
//                 stack_pop(&(proc.stack), &arg);
//
//                 proc.RAM[proc.regs[proc.bin_code[++proc.C_E]]] = arg;
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_PAINT:
//             {
//                 int x = 0;
//                 int y = 0;
//
//                 stack_pop(&(proc.stack), &y);
//                 stack_pop(&(proc.stack), &x);
//
//                 int address = y * disp_set.len + x;
//                 int color = proc.bin_code[++proc.C_E];
//
//                 (proc.VRAM)[address] = color;
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_COLOR:
//             {
//                 int x = 0;
//                 int y = 0;
//
//                 stack_pop(&(proc.stack), &y);
//                 stack_pop(&(proc.stack), &x);
//
//                 int address = y * disp_set.len + x;
//                 int color = (proc.VRAM)[address];
//
//                 stack_push(&(proc.stack), color);
//
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_DRAW:
//             {
//                 system("clear");
//                 for (int i = 0; i < disp_set.size; i++) {
//                     putchar(proc.VRAM[i]);
//
//                     if (i % disp_set.len == 0) {
//                         putchar('\n');
//                     }
//                 }
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             case INT_CTIME:
//             {
//                 struct timeval tv;
//                 gettimeofday(&tv, NULL);
//
//                 long long milisec = ((long long)tv.tv_sec * 1000 + tv.tv_usec / 1000) % TIME_DIV;
//                 stack_push(&(proc.stack), (int) milisec);
//                 break;
//             }
// //----------------------------------------------------------------------------------------------------
//             default:
//             {
//                 cpu_dtor(&proc);
//                 printf(_R_ "ERROR: unknown command\n" _N_);
//                 return -1;
//             }
//         }
        int comand = proc.bin_code[proc.C_E];
        if (comand >= AMOUNT_CMD || comand < 0)
        {
            printf("ERROR: unknown comand in bin_code\n");
            cpu_dtor(&proc);
            return 0;
        }

        if ((CMD_INF[comand].func(&proc, CMD_INF[comand].number)) != P_OK)
        {
            cpu_dtor(&proc);
            return 0;
        }
    }
    printf(_R_ "Invalid go???\n" _N_);
    cpu_dtor(&proc);
    return -1;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int cpu_ctor(cpu_t* proc,
             const char* name_bin_file,
             int* amount_elements) {
    assert(proc);
    assert(name_bin_file);

    int* bin_code = create_bin_buffer(name_bin_file,
                                      amount_elements,
                                      &(proc->disp_set),
                                      &(proc->audio_set));
    if (bin_code == NULL) {
        return -1;
    }
    proc->bin_code = bin_code;

    int* RAM_address = create_int_buffer(RAM_SIZE);
    if (RAM_address == NULL) {
        return -1;
    }
    proc->RAM = RAM_address;

    int* VRAM_address = create_int_buffer((size_t) proc->disp_set.size);
    if (VRAM_address == NULL) {
        return -1;
    }
    proc->VRAM = VRAM_address;

    for (int i = 0; i < proc->disp_set.size; i++) {
        VRAM_address[i] = 0;
    }

    int size_stack = START_SIZE_STACK;

    if (stack_creator(&(proc->stack), size_stack, __FILE__,  __LINE__, NAME_RETURN(stack)) != 0) {
        printf(_R_ "ERROR: creating stack was not completed\n" _N_);
        return -1;
    }

    if (stack_creator(&(proc->address), 4, __FILE__,  __LINE__, NAME_RETURN(labels)) != 0) {
        printf(_R_ "ERROR: creating stack was not completed\n" _N_);
        return -1;
    }

    if (create_display(&(proc->disp_set)) != P_OK)
    {
        return -1;
    }

    if (create_sound(&(proc->audio_set)) != P_OK)
    {
        return -1;
    }

    for (int i = 0; i < AMOUNT_REGISTERS; i++) {
        proc->regs[i] = 0;
    }

    proc->C_E = 0;
    proc->amount_el = *amount_elements;

    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int cpu_dtor(cpu_t* proc) {
    assert(proc);

    stack_destruct(&(proc->stack));
    stack_destruct(&(proc->address));

    destroy_display(proc);

    free(proc->RAM);
    free(proc->VRAM);
    free(proc->bin_code);

    SDL_CloseAudioDevice(proc->audio_set.dev);
    SDL_Quit();
    free(proc->audio_set.audio_file);

    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int proc_dump(cpu_t* proc) {
    assert(proc);

    // stack_dump(&(proc->stack)); // Выводит слишком много
    // printf(_R_ "\n=== STACK ===\n\n" _N_);

    printf(_P_"size = %s%zd%s\n", _B_, proc->stack.size, _P_);
    // printf("capacity = %s%zd%s\n", _B_, proc->stack.capacity, _P_);
    // printf("data %s[%p]%s\n" _N_ , _B_, proc->stack.data, _P_);

    print_stack_for_dump(&(proc->stack), NOT_ERRORS);

    printf(_R_ "\n=== BIN_CODE ===\n\n" _N_);

    print_before_end(proc);
    print_end(proc);

    printf(_R_ "\n=== Registers ===\n\n");
    print_reg(proc);

    // printf(_R_ "\n=== Registers ===\n\n");


    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int print_before_end(cpu_t* proc) {
    assert(proc);

    for (unsigned int i = 0; i < ((unsigned) (proc->amount_el - AMOUNT_SUP_NUM) / 8); i++) {
        print_line(proc, i);
    }

    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int print_end(cpu_t* proc) {
    assert(proc);

    // print_line(proc, ((unsigned) (proc->amount_el - AMOUNT_SUP_NUM) % 8 ));
    printf(_G_ "[%p]: " _N_, proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8);

    for (int i = 0; i < (proc->amount_el - AMOUNT_SUP_NUM) % 8; i++) {
        if ((((proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8) + i) == proc->C_E ) {
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 0));
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 1));
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 2));
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 3));
        }

        else if (i % 2 == 0) {
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 0));
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 1));
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 2));
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 3));
        }
        else {
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 0));
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 1));
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 2));
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + (proc->amount_el - AMOUNT_SUP_NUM) / 8 * 8 + i) + 3));
        }
    }
    // for (unsigned i = 0; i < ((unsigned) (proc->amount_el - AMOUNT_SUP_NUM) % 8); i++) {
    //     print_line(proc, ((unsigned) (proc->amount_el - AMOUNT_SUP_NUM) / 8 - 1));
    // }

    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int print_line(cpu_t* proc, unsigned int i) {
    assert(proc);

    printf(_G_ "[%p]: " _N_, proc->bin_code + i * 8);

    for (unsigned int buf = 0; buf < 8; buf++) {
        if (i * 8 + buf == (unsigned) proc->C_E ) {
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 0));
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 1));
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 2));
            printf(_G_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 3));
        }

        else if (buf % 2 == 0) {
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 0));
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 1));
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 2));
            printf(_B_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 3));
        }

        else {
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 0));
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 1));
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 2));
            printf(_P_ "%02X " _N_, *((unsigned char*) (proc->bin_code + i * 8 + buf) + 3));
        }

    }
    putchar('\n');
    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int print_reg(cpu_t* proc) {
    assert(proc);

    for (int i = 0; i < AMOUNT_REGISTERS; i++) {
        printf(_P_ "%s: %s%08d\n" _N_, CHAR_REG[i], _B_, proc->regs[i]);
    }

    return 0;
}
// -------------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------------
int check_correct_func(void)
{
    for (int i = 0; i < AMOUNT_CMD - 1; i++)
    {
        if (CMD_INF[i].number >= CMD_INF[i + 1].number)
        {
            printf("%d: %d %d: %d\n", i, CMD_INF[i].number, i + 1,  CMD_INF[i + 1].number);
            return P_ERROR;
        }
    }
    return P_OK;
}


