#include <stdint.h>
#include <string.h>

#include "instruction.h"
#include "emulator.h"
#include "emulator_function.h"
#include "modrm.h"

instruction_func_t* instructions[256];

static void mov_r32_imm32(Emulator* emu){
  uint8_t reg = get_code8(emu, 0) - 0xB8;
  uint32_t value = get_code32(emu, 1);
  printf("mov_r32_imm32:reg:%x\n", reg);
  printf("mov_r32_imm32:value:%x\n", value);
  emu->registers[reg] = value;
  emu->eip += 5;
}

static void mov_rm32_imm32(Emulator* emu){
  emu->eip += 1;
  ModRM modrm;
  parse_modrm(emu, &modrm);
  uint32_t value = get_code32(emu, 0);
  emu->eip += 4;
  set_rm32(emu, &modrm, value);
}

static void short_jump(Emulator* emu){
  int8_t diff = get_sign_code8(emu, 1);
  printf("short_jump:diff:%x\n", diff);
  printf("short_jump:%x\n", emu->eip + (diff + 2));
  emu->eip += (diff + 2);
}

static void near_jump(Emulator* emu){
  int32_t diff = get_sign_code32(emu, 1);
  printf("near_jump:diff:%x\n", diff);
  printf("near_jump:%x\n", emu->eip + (diff + 5));
  emu->eip += (diff + 5);
}

void init_instructions(void){
  int i;
  memset(instructions, 0, sizeof(instructions));
   for(i = 0; i < 8; i++){
    instructions[0xB8 + i] = mov_r32_imm32;
  }
  instructions[0xE9] = near_jump;
  instructions[0xEB] = short_jump;
}
