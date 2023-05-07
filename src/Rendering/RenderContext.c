#include "RenderContext.h"

int init_render_context(RenderContext *context)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        fprintf(stderr, "ERROR: Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    context->window = SDL_CreateWindow("chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_RESIZABLE);

    if (!context->window)
    {
        fprintf(stderr, "ERROR: Failed to create window: %s\n", SDL_GetError());
        return 1;
    }

    context->renderer = SDL_CreateRenderer(context->window, -1, 0);

    if (!context->renderer)
    {
        fprintf(stderr, "ERROR: Failed to create renderer: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

void dispose_render_context(RenderContext *context)
{
    if (context->renderer != NULL)
    {
        SDL_DestroyRenderer(context->renderer);
        context->renderer = NULL;
    }

    if (context->window != NULL)
    {
        SDL_DestroyWindow(context->window);
        context->window = NULL;
    }

    SDL_Quit();
}