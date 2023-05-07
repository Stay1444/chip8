#pragma once

#include <stdbool.h>

#define VM_DISPLAY_WIDTH 64
#define VM_DISPLAY_HEIGHT 32

typedef struct
{
    bool pixels[VM_DISPLAY_HEIGHT][VM_DISPLAY_WIDTH];
} Display;

void display_clear(Display *display);