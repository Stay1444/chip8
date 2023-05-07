#include "DisplayRenderer.h"
#include "math.h"

#define SCREEN_MARGIN 15
#define PIXEL_MARGIN 0

void render_display(RenderContext *context, Display *display)
{
    int window_width, window_height;
    SDL_GetWindowSize(context->window, &window_width, &window_height);

    float display_width, display_height;
    float aspect_ratio = (float)VM_DISPLAY_WIDTH / (float)VM_DISPLAY_HEIGHT;
    if ((float)window_width / (float)window_height > aspect_ratio)
    {
        display_width = (float)window_height * aspect_ratio;
        display_height = (float)window_height;
    }
    else
    {
        display_width = (float)window_width;
        display_height = (float)window_width / aspect_ratio;
    }

    float left_margin = roundf(((float)window_width - display_width) / 2.0f - SCREEN_MARGIN);
    float top_margin = roundf(((float)window_height - display_height) / 2.0f - SCREEN_MARGIN);

    SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(context->renderer, NULL);

    for (int y = 0; y < VM_DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < VM_DISPLAY_WIDTH; x++)
        {
            if (display->pixels[y][x])
            {
                SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(context->renderer, 10, 10, 10, 255);
            }

            int pixel_x = (int)roundf(left_margin + SCREEN_MARGIN + PIXEL_MARGIN + ((float)display_width / (float)VM_DISPLAY_WIDTH) * (float)x);
            int pixel_y = (int)roundf(top_margin + SCREEN_MARGIN + PIXEL_MARGIN + ((float)display_height / (float)VM_DISPLAY_HEIGHT) * (float)y);
            int pixel_w = (int)roundf((float)display_width / (float)VM_DISPLAY_WIDTH) - 2 * PIXEL_MARGIN;
            int pixel_h = (int)roundf((float)display_height / (float)VM_DISPLAY_HEIGHT) - 2 * PIXEL_MARGIN;

            SDL_Rect pixel_rect = {pixel_x, pixel_y, pixel_w, pixel_h};
            SDL_RenderFillRect(context->renderer, &pixel_rect);
        }
    }

    SDL_RenderPresent(context->renderer);
}