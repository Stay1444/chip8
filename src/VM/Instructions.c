#include "Instructions.h"

#include "stdint.h"

const char *opcode_to_cstr(uint16_t opcode)
{
    switch (opcode)
    {
    case INST_CLEAR_SCREEN:
        return "INST_CLEAR_SCREEN";
    case INST_JUMP:
        return "INST_JUMP";
    case INST_SETVX:
        return "INST_SETVX";
    case INST_ADDVX:
        return "INST_ADDVX";
    case INST_SETIR:
        return "INST_SETIR";
    case INST_DRAW:
        return "INST_DRAW";
    default:
        return "UNKNOWN OPCODE in opcode_to_cstr";
    }
}