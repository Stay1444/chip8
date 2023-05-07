#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "Display.h"
#include "Stack.h"
#include "Keyboard.h"
#define VM_MEMORY_SIZE 4096
#define VM_VARIABLE_REGISTER_COUNT 16

typedef uint16_t INST;

typedef enum VMError
{
    VMERROR_OK = 0,
    VMERROR_STACK_OVERFLOW,
    VMERROR_STACK_UNDERFLOW,
    VMERROR_UNSUPPORTED_OPCODE
} VMError;

const char *vmerror_to_cstr(VMError error);

typedef struct
{
    uint8_t memory[VM_MEMORY_SIZE];
    Display display;
    size_t program_counter;
    uint16_t index_register;
    Stack stack;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t variable_registers[VM_VARIABLE_REGISTER_COUNT];
} VM;

VM *vm_new(void);
void vm_free(VM *vm);

void vm_memcpy(VM *vm, size_t start, void *source, size_t length);
int vm_load_program(VM *vm, const char *filename);
INST vm_fetch(VM *vm);
VMError vm_execute(VM *vm, Keyboard *keyboard);