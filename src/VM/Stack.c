#include "Stack.h"

int stack_push(Stack *stack, uint16_t value)
{
    if (stack->top < VM_STACK_SIZE - 1)
    {
        stack->data[++stack->top] = value;
        return VMERROR_OK;
    }
    else
    {
        return VMERROR_STACK_UNDERFLOW;
    }
}

int stack_pop(Stack *stack, uint16_t *value)
{
    if (stack->top >= 0)
    {
        *value = stack->data[stack->top--];
        return VMERROR_OK;
    }
    else
    {
        return VMERROR_STACK_OVERFLOW;
    }
}