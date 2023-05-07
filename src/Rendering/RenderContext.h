#pragma once

#include "SDL2/SDL.h"
#include <stdio.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;

} RenderContext;

int init_render_context(RenderContext *context);
void dispose_render_context(RenderContext *context);