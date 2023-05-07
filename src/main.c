#include <stdio.h>
#include <string.h>
#include <time.h>

#include "SDL2/SDL.h"

#include "Data/Font.h"

#include "VM/VM.c"

#include "Rendering/RenderContext.c"
#include "Rendering/DisplayRenderer.c"

#define TARGET_FPS 60
#define TARGET_IPS 500

void update_keyboard(SDL_Event *event, Keyboard *keyboard);

int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));
    if (argc < 2)
    {
        printf("Usage: chip8 <path-to-rom>\n");
        return 0;
    }

    const char *file_path = argv[1];

    VM *vm = vm_new();

    vm_memcpy(vm, 0x0, (void *)FONT_DATA, FONT_DATA_SIZE);

    printf("Loading program %s\n", file_path);

    if (vm_load_program(vm, file_path) != 0)
    {
        return 1;
    }

    vm->program_counter = 0x200;

    printf("Initializing graphics\n");

    RenderContext render_context = {0};

    if (init_render_context(&render_context) != 0)
    {
        fprintf(stderr, "ERROR: Failed to initialize graphics.");
        return 1;
    }

    SDL_Event event;

    int running = 1;

    uint64_t NOW = SDL_GetPerformanceCounter();
    uint64_t LAST = 0;
    double deltaTime = 0;

    double drawTimer = 0;
    double instructionTimer = 0;
    double secondTimer = 0;

    int drawTimes = 0;
    int instructionTimes = 0;
    char *title = malloc(32 * sizeof(char));

    Keyboard keyboard = {0};

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
            {
                update_keyboard(&event, &keyboard);
            }
        }

        NOW = SDL_GetPerformanceCounter();

        deltaTime = (double)((double)(NOW - LAST) * (double)1000.0 / (double)SDL_GetPerformanceFrequency());
        LAST = NOW;

        drawTimer += deltaTime;
        secondTimer += deltaTime;
        instructionTimer += deltaTime;

        if (drawTimer > 1000 / TARGET_FPS)
        {
            drawTimer = 0;
            // Decrease timers 60 times per second
            if (vm->delay_timer > 0)
            {
                vm->delay_timer -= 1;
            }

            if (vm->sound_timer > 0)
            {
                vm->sound_timer -= 1;
            }

            render_display(&render_context, &vm->display);
            drawTimes += 1;
        }

        if (instructionTimer > 1000.0 / TARGET_IPS)
        {
            instructionTimer = 0;
            VMError error = vm_execute(vm, &keyboard);
            if (error != VMERROR_OK)
            {
                fprintf(stderr, "ERROR: %s\n", vmerror_to_cstr(error));
                running = 0;
            }
            instructionTimes += 1;
        }

        if (secondTimer > 1000)
        {
            snprintf(title, 32, "chip8 - FPS: %i | IPS: %i", drawTimes, instructionTimes);
            SDL_SetWindowTitle(render_context.window, title);
            secondTimer = 0;
            drawTimes = 0;
            instructionTimes = 0;
        }

        SDL_RenderPresent(render_context.renderer);
    }

    free(title);
    vm_free(vm);
    printf("Disposing graphics");
    dispose_render_context(&render_context);

    return 0;
}

void update_keyboard(SDL_Event *event, Keyboard *keyboard)
{
    int keydown = event->type == SDL_KEYDOWN;
    switch (event->key.keysym.sym)
    {
    case SDLK_1:
        keyboard->keys[0x1] = keydown > 0;
        break;
    case SDLK_2:
        keyboard->keys[0x2] = keydown > 0;
        break;
    case SDLK_3:
        keyboard->keys[0x3] = keydown > 0;
        break;
    case SDLK_4:
        keyboard->keys[0xc] = keydown > 0;
        break;
    case SDLK_q:
        keyboard->keys[0x4] = keydown > 0;
        break;
    case SDLK_w:
        keyboard->keys[0x5] = keydown > 0;
        break;
    case SDLK_e:
        keyboard->keys[0x6] = keydown > 0;
        break;
    case SDLK_r:
        keyboard->keys[0xD] = keydown > 0;
        break;
    case SDLK_a:
        keyboard->keys[0x7] = keydown > 0;
        break;
    case SDLK_s:
        keyboard->keys[0x8] = keydown > 0;
        break;
    case SDLK_d:
        keyboard->keys[0x9] = keydown > 0;
        break;
    case SDLK_f:
        keyboard->keys[0xE] = keydown > 0;
        break;
    case SDLK_z:
        keyboard->keys[0xA] = keydown > 0;
        break;
    case SDLK_x:
        keyboard->keys[0x0] = keydown > 0;
        break;
    case SDLK_c:
        keyboard->keys[0xB] = keydown > 0;
        break;
    case SDLK_v:
        keyboard->keys[0xF] = keydown > 0;
        break;
    default:
        break;
    }
}
