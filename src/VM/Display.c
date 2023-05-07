#include "Display.h"

void display_clear(Display *display)
{
    for (int x = 0; x < VM_DISPLAY_WIDTH; x++)
    {
        for (int y = 0; y < VM_DISPLAY_HEIGHT; y++)
        {
            display->pixels[y][x] = 0;
        }
    }
}