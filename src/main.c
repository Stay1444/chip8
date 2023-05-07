#include <stdio.h>
#include <string.h>

#include "SDL2/SDL.h"

#include "Data/Font.h"

#include "VM/VM.c"

#include "Rendering/RenderContext.c"
#include "Rendering/DisplayRenderer.c"

#define TARGET_FPS 60
#define TARGET_IPS 25

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Usage: chip8 <path-to-rom>\n");
        return 0;
    }

    const char *file_path = argv[1];

    VM *vm = vm_new();

    vm_memcpy(vm, 0x50, (void *)FONT_DATA, FONT_DATA_SIZE);

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

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
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
            VMError error = vm_execute(vm);
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