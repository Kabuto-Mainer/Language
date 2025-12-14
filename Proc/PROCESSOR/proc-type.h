#ifndef PROC_TYPE_H
#define PROC_TYPE_H

#include <stdlib.h>
#include <SDL2/SDL.h>
#include "stack_define.h"


//------------------------------------------------------------------------------------------------
struct display_t
{
    int len;
    int high;
    int size;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};
//------------------------------------------------------------------------------------------------
struct audio_t
{
    SDL_AudioSpec audio;
    SDL_AudioDeviceID dev;

    int sound_stream;
    int code_stream;

    int amount_frames;
    int16_t* audio_file;
};


//------------------------------------------------------------------------------------------------
struct cpu_t
{
    stack_struct stack;
    stack_struct address;
    display_t disp_set;
    audio_t audio_set;
    int* RAM;
    int* VRAM;
    int* bin_code;
    int C_E;
    int regs[AMOUNT_REGISTERS];
    int amount_el;
};
//------------------------------------------------------------------------------------------------
typedef int (*cpu_func)(cpu_t*, const int );
//------------------------------------------------------------------------------------------------


#endif // PROC_TYPE_H
