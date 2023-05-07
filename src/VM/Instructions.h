#pragma once

#define INST_CLEAR_SCREEN 0x0000
#define INST_JUMP 0x1000
#define INST_SETVX 0x6000
#define INST_ADDVX 0x7000
#define INST_SETIR 0xA000
#define INST_DRAW 0xD000
#define INST_SCALL 0x2000
#define INST_SRET 0x00EE

#define INST_SKIP_EQ 0x3000
#define INST_SKIP_NOT_EQ 0x4000
#define INST_SKIP_V_EQ 0x5000
#define INST_SKIP_V_NOT_EQ 0x9000

#define INST_MATH 0x8000

#define INST_JUMP_OFFSET 0xB000
#define INST_RANDOM 0xC000

#define INST_SKIP_IF_KEY 0xE000
#define INST_TIMER 0xF000
const char *opcode_to_cstr(uint16_t opcode);