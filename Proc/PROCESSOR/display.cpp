#include <stdlib.h>
#include <assert.h>
#include <SDL2/SDL.h>

// #include "processor.h"
#include "proc-type.h"
#include "../COMMON/comand.h"
#include "../COMMON/config.h"
#include "../COMMON/support.h"
#include "display.h"


int create_display(display_t* disp_set)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return P_ERROR;
    }

    SDL_Window* window = SDL_CreateWindow("Display",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          disp_set->len * PIXEL_SIZE, disp_set->high * PIXEL_SIZE,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

      if (!renderer) {
        printf("Renderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return P_ERROR;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             disp_set->len, disp_set->high);

    disp_set->window = window;
    disp_set->renderer = renderer;
    disp_set->texture = texture;

//     if (SDL_Init(SDL_INIT_AUDIO) < 0) {
//         printf("SDL_Init error: %s\n", SDL_GetError());
//         return P_ERROR;
//     }
//
//     SDL_AudioSpec want, have;
//     SDL_zero(want);
//
//     want.freq = 44100;              // частота дискретизации
//     want.format = AUDIO_S16SYS;     // формат (signed 16-bit integer)
//     want.channels = 2;              // моно
//     want.samples = 1024;            // размер буфера
//     want.callback = NULL;           // используем push API (QueueAudio)
//
//     SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
//     if (!dev) {
//         printf("SDL_OpenAudioDevice error: %s\n", SDL_GetError());
//         return P_ERROR;
//     }
//
//     disp_set->audio = have;
//     disp_set->dev = dev;
//
// // TODO не забыть добавить проверку на код
//
//     if (disp_set->sound_stream != -1)
//     {
//         FILE* file_stream = fopen_file(NAME_SOUND_STREAM[disp_set->sound_stream], "rb");
//         size_t size_sempls = 0;
//
//         size_t trash_1 = fread(&size_sempls, sizeof(int), 1, file_stream);
//         (void) trash_1;
//
//         printf("SIZE_SEMPLES: %zu\n", size_sempls);
//
//         int16_t* sempls = (int16_t*) calloc(size_sempls, sizeof(int16_t));
//         size_t trash_2 = fread(sempls, sizeof(int16_t), size_sempls, file_stream);
//         (void) trash_2;
//
//         fclose_file(file_stream);
//         disp_set->audio_file = sempls;
//     }

    return P_OK;
}

int create_sound(audio_t* audio_set)
{
    assert(audio_set);

        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return P_ERROR;
    }

    SDL_AudioSpec want, have;
    SDL_zero(want);

    want.freq = U_AUDIO;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 1024;
    want.callback = NULL;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (!dev) {
        printf("SDL_OpenAudioDevice error: %s\n", SDL_GetError());
        return P_ERROR;
    }

    audio_set->audio = have;
    audio_set->dev = dev;

// TODO не забыть добавить проверку на код

    if (audio_set->sound_stream != -1)
    {
        FILE* file_stream = fopen_file(NAME_SOUND_STREAM[audio_set->sound_stream], "rb");
        int code_stream = 0;
        size_t trash_3 = fread(&code_stream, sizeof(int), 1, file_stream);
        (void) trash_3;

        if (code_stream != audio_set->code_stream)
        {
            printf("ERROR: code stream is not correct\n");
            return P_ERROR;
        }

        size_t size_sempls = 0;
        size_t trash_1 = fread(&size_sempls, sizeof(int), 1, file_stream);
        (void) trash_1;

        int16_t* sempls = (int16_t*) calloc(size_sempls, sizeof(int16_t));
        size_t trash_2 = fread(sempls, sizeof(int16_t), size_sempls, file_stream);
        (void) trash_2;

        fclose_file(file_stream);
        audio_set->audio_file = sempls;
    }

    return P_OK;
}

int update_display(cpu_t* proc)
{
    assert(proc);

    SDL_Event cur_event = {};
    SDL_PollEvent(&cur_event);

    if (cur_event.type == SDL_QUIT)
    {
        return P_END;
    }

    SDL_UpdateTexture(proc->disp_set.texture,
                      NULL,
                      proc->VRAM,
                      (long unsigned int) proc->disp_set.len * sizeof(int));

    SDL_RenderClear(proc->disp_set.renderer);
    SDL_RenderCopy(proc->disp_set.renderer,
                   proc->disp_set.texture,
                   NULL,
                   NULL);
    SDL_RenderPresent(proc->disp_set.renderer);

    // SDL_Delay(10);

    return P_OK;
}


int destroy_display(cpu_t* proc)
{
    assert(proc);

    SDL_DestroyTexture(proc->disp_set.texture);
    SDL_DestroyRenderer(proc->disp_set.renderer);
    SDL_DestroyWindow(proc->disp_set.window);
    SDL_Quit();

    return P_OK;
}




