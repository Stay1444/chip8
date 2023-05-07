#pragma once

#include "VM.h"
#include "Instructions.c"
#include <stdio.h>
#include "Display.c"
#include "Stack.c"
#include "Keyboard.h"

#define CHIP8_SHIFT_LEGACY_BEHAVIOR 0
#define CHIP48_BEHAVIOR 1

const char *vmerror_to_cstr(VMError error)
{
    switch (error)
    {
    case VMERROR_OK:
        return "VMERROR_OK";
    case VMERROR_STACK_OVERFLOW:
        return "VMERROR_STACK_OVERFLOW";
    case VMERROR_STACK_UNDERFLOW:
        return "VMERROR_STACK_UNDERFLOW";
    case VMERROR_UNSUPPORTED_OPCODE:
        return "VMERROR_UNSUPPORTED_OPCODE";
    default:
        return "vmerror_to_cstr unknown error";
        break;
    }
}

VM *vm_new(void)
{
    VM *vm = calloc(1, sizeof(VM));
    memset(vm->memory, 0, sizeof(vm->memory));
    memset(vm->variable_registers, 0, sizeof(vm->variable_registers));

    if (vm == NULL)
    {
        return NULL;
    }

    return vm;
}

void vm_free(VM *vm)
{
    if (vm != NULL)
    {
        memset(vm, 0, sizeof(VM));
        free(vm);
    }
}

void vm_memcpy(VM *vm, size_t start, void *source, size_t length)
{
    if (start + length > VM_MEMORY_SIZE)
    {
        fprintf(stderr, "Error: Memory copy out of bounds!\n");
        return;
    }

    memcpy(&vm->memory[start], source, length);
}

int vm_load_program(VM *vm, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open file %s.\n", filename);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = (size_t)ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size > VM_MEMORY_SIZE - 0x200)
    {
        fprintf(stderr, "Error: File %s is too large to load into memory.\n", filename);
        fclose(file);
        return 1;
    }

    uint8_t *buffer = (uint8_t *)malloc(file_size);
    if (buffer == NULL)
    {
        fprintf(stderr, "Error: Unable to allocate memory for file %s.\n", filename);
        fclose(file);
        return 1;
    }

    fread(buffer, 1, file_size, file);
    fclose(file);

    vm_memcpy(vm, 0x200, buffer, file_size);
    free(buffer);
    return 0;
}

INST vm_fetch(VM *vm)
{
    INST instruction = (vm->memory[vm->program_counter] << 8) | vm->memory[vm->program_counter + 1];
    return instruction;
}

VMError vm_execute(VM *vm, Keyboard *keyboard)
{
    INST instruction = vm_fetch(vm);

    uint16_t opcode = instruction & 0xF000;
    uint8_t X = (instruction & 0x0F00) >> 8;
    uint8_t Y = (instruction & 0x00F0) >> 4;

    uint8_t N = (uint8_t)(instruction & 0x000F);
    uint8_t NN = (uint8_t)(instruction & 0x00FF);
    uint16_t NNN = (instruction & 0x0FFF);

    size_t pcIncrement = 2;
    switch (opcode)
    {
    case INST_CLEAR_SCREEN:
        if (instruction == 0x00E0)
        {
            display_clear(&vm->display);
        }
        else if (instruction == INST_SRET)
        {
            uint16_t v;
            VMError error = stack_pop(&vm->stack, &v);
            if (error != VMERROR_OK)
            {
                return error;
            }
            vm->program_counter = v;
            pcIncrement = 0;
        }
        break;
    case INST_JUMP:
        vm->program_counter = NNN;
        pcIncrement = 0;
        break;
    case INST_SETVX:
        vm->variable_registers[X] = NN;
        break;
    case INST_ADDVX:
        vm->variable_registers[X] += NN;
        break;
    case INST_SETIR:
        vm->index_register = NNN;
        break;
    case INST_DRAW:
    {
        int dx = vm->variable_registers[X] % VM_DISPLAY_WIDTH;
        int dy = vm->variable_registers[Y] % VM_DISPLAY_HEIGHT;
        int height = instruction & 0x000F;
#if DEBUG
        printf("Drawing X: %i DX: %i Y: %i DY: %i HEIGHT: %i\n", X, dx, Y, dy, height);
#endif
        int pixel;
        vm->variable_registers[0xF] = 0;

        for (int y = 0; y < height; y++)
        {
            pixel = vm->memory[vm->index_register + y];
            for (int x = 0; x < 8; x++)
            {
                if (pixel & (0x80 >> x))
                {
                    int draw_x = (dx + x) % VM_DISPLAY_WIDTH;
                    int draw_y = (dy + y) % VM_DISPLAY_HEIGHT;
                    if (vm->display.pixels[draw_y][draw_x] == 1)
                    {
                        vm->variable_registers[0xF] = 1;
                    }
                    vm->display.pixels[draw_y][draw_x] ^= 1;
                }
            }
        }
        break;
    }
    case INST_SCALL:
    {
        stack_push(&vm->stack, (uint16_t)(vm->program_counter + pcIncrement));
        pcIncrement = 0;
        vm->program_counter = NNN;
        break;
    }
    case INST_SKIP_EQ:
    {
        if (vm->variable_registers[X] == NN)
        {
            pcIncrement += 2;
        }
        break;
    }
    case INST_SKIP_NOT_EQ:
    {
        if (vm->variable_registers[X] != NN)
        {
            pcIncrement += 2;
        }
        break;
    }
    case INST_SKIP_V_EQ:
    {
        if (vm->variable_registers[X] == vm->variable_registers[Y])
        {
            pcIncrement += 2;
        }
        break;
    }
    case INST_SKIP_V_NOT_EQ:
    {
        if (vm->variable_registers[X] != vm->variable_registers[Y])
        {
            pcIncrement += 2;
        }
        break;
    }
    case INST_MATH:
    {
        switch (N)
        {
        case 0x0:
            vm->variable_registers[X] = vm->variable_registers[Y];
            break;
        case 0x1:
            vm->variable_registers[X] = vm->variable_registers[X] | vm->variable_registers[Y];
            break;
        case 0x2:
            vm->variable_registers[X] = vm->variable_registers[X] & vm->variable_registers[Y];
            break;
        case 0x3:
            vm->variable_registers[X] = vm->variable_registers[X] ^ vm->variable_registers[Y];
            break;
        case 0x4:
        {
            if (vm->variable_registers[X] > (UINT8_MAX - vm->variable_registers[Y]))
            {
                vm->variable_registers[0xF] = 1;
            }
            else
            {
                vm->variable_registers[0xF] = 0;
            }
            vm->variable_registers[X] = vm->variable_registers[X] + vm->variable_registers[Y];
            break;
        }
        case 0x5:
        {
            if (vm->variable_registers[X] > vm->variable_registers[Y])
            {
                vm->variable_registers[0xF] = 1;
            }
            else
            {
                vm->variable_registers[0xF] = 0;
            }
            vm->variable_registers[X] = vm->variable_registers[X] - vm->variable_registers[Y];
            break;
        }
        case 0x7:
        {
            if (vm->variable_registers[Y] > vm->variable_registers[X])
            {
                vm->variable_registers[0xF] = 1;
            }
            else
            {
                vm->variable_registers[0xF] = 0;
            }
            vm->variable_registers[X] = vm->variable_registers[Y] - vm->variable_registers[X];

            break;
        }
        case 0x6:
        {
#if CHIP8_SHIFT_LEGACY_BEHAVIOR == 0
            vm->variable_registers[0xF] = vm->variable_registers[X] & 0x01; // set VF to the bit shifted out
            vm->variable_registers[X] = vm->variable_registers[X] >> 1;     // shift right
#else
            vm->variable_registers[0xF] = vm->variable_registers[X] & 0x01; // set VF to the bit shifted out
            vm->variable_registers[X] = vm->variable_registers[Y] >> 1;     // set VX to VY and shift right
#endif
            break;
        }
        case 0xE:
        {
#if CHIP8_SHIFT_LEGACY_BEHAVIOR == 0
            vm->variable_registers[0xF] = vm->variable_registers[X] & 0x01; // set VF to the bit shifted out
            vm->variable_registers[X] = vm->variable_registers[X] << 1;     // shift left
#else
            vm->variable_registers[0xF] = vm->variable_registers[X] & 0x01; // set VF to the bit shifted out
            vm->variable_registers[X] = vm->variable_registers[Y] << 1;     // set VX to VY and shift left
#endif
            break;
        }
        default:
            printf("Unknown math/arithmetic operation %i\n", N);
            break;
        }
        break;
    }
    case INST_JUMP_OFFSET:
    {
#if CHIP48_BEHAVIOR == 1
        // Jump to address XNN + value in register VX
        vm->program_counter = vm->variable_registers[X] + (opcode & 0x0FFF);
#else
        // Jump to address NNN + value in register V0
        vm->program_counter = vm->variable_registers[0] + (opcode & 0x0FFF);
#endif
        pcIncrement = 0;
        break;
    }
    case INST_RANDOM:
    {
        vm->variable_registers[X] = (uint8_t)((rand() % 256) & NN);
        break;
    }
    case INST_SKIP_IF_KEY:
    {
        switch (NN)
        {
        case 0x9E:
        {
            if (keyboard->keys[vm->variable_registers[X]])
            {
                pcIncrement += 2;
            }
            break;
        }
        case 0xA1:
        {
            if (!keyboard->keys[vm->variable_registers[X]])
            {
                pcIncrement += 2;
            }
            break;
        }

        default:
            printf("UNIMPLEMENTED INST_SKIP_KEY %i\n", NN);
            break;
        }
        break;
    }
    case INST_TIMER:
    {
        switch (NN)
        {
        case 0x7:
            vm->variable_registers[X] = vm->delay_timer;
            break;
        case 0x15:
            vm->delay_timer = vm->variable_registers[X];
            break;
        case 0x18:
            vm->sound_timer = vm->variable_registers[X];
            break;
        case 0x1E:
        {
            vm->index_register += vm->variable_registers[X];
            if (vm->index_register > 0xFFF)
            {
                vm->variable_registers[0xF] = 1;
            }
            else
            {
                vm->variable_registers[0XF] = 0;
            }
            break;
        }
        case 0x0A:
        {
            if (!keyboard->keys[vm->variable_registers[X]])
            {
                pcIncrement -= 2;
            }
            // printf("%i | %i\n", vm->variable_registers[X], keyboard->keys[vm->variable_registers[X]]);
            break;
        }
        case 0x29:
        {

            uint8_t character = vm->variable_registers[X] & 0x0F;
            uint16_t address = character * 5;
            vm->index_register = address;
            break;
        }
        case 0x33:
        {
            uint8_t v = vm->variable_registers[X];

            uint8_t ones = v % 10;
            v /= 10;
            uint8_t tens = v % 10;
            v /= 10;
            uint8_t hundreds = v % 10;

            vm->memory[vm->index_register] = hundreds;
            vm->memory[vm->index_register + 1] = tens;
            vm->memory[vm->index_register + 2] = ones;
            break;
        }
        case 0x55:
        {
            for (int i = 0; i <= X; i++)
            {
                vm->memory[vm->index_register + i] = vm->variable_registers[i];
            }
            break;
        }
        case 0x65:
        {
            for (int i = 0; i <= X; i++)
            {
                vm->variable_registers[i] = vm->memory[vm->index_register + i];
            }
            break;
        }
        default:
            printf("Unknown timer %i\n", NN);
            return VMERROR_UNSUPPORTED_OPCODE;
        }
        break;
    }

    default:
        return VMERROR_UNSUPPORTED_OPCODE;
    }

    vm->program_counter += (size_t)pcIncrement;
    return VMERROR_OK;
}