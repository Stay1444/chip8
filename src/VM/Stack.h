#pragma once

#define VM_STACK_SIZE 16

#include <stdint.h>

#include "VM.h"

typedef struct
{
    uint16_t data[VM_STACK_SIZE];
    int top;
} Stack;

int stack_push(Stack *stack, uint16_t value);
int stack_pop(Stack *stack, uint16_t *value);