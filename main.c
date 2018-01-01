#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "emulator.h"
#include "emulator_function.h"
#include "instruction.h"

#define REGISTERS_COUNT 16
#define ESP 1
#define MEMORY_SIZE 0x9c00

Emulator* create_emu(size_t size, uint32_t eip, uint32_t esp){
  Emulator* emu = malloc(sizeof(Emulator));
  emu->memory = malloc(size);
  memset(emu->registers, 0, sizeof(emu->registers));
  emu->eip = eip;
  emu->registers[ESP] = esp;
  return emu;
}

void destroy_emu(Emulator* emu){
  free(emu->memory);
  free(emu);
}

void dump_registers(Emulator* emu){
  printf("dump:0x00:%x\n", emu->registers[0x00]);
  printf("dump:0x01:%x\n", emu->registers[0x01]);
  printf("dump:0x02:%x\n", emu->registers[0x02]);
  printf("dump:0x03:%x\n", emu->registers[0x03]);
  printf("dump:0x04:%x\n", emu->registers[0x04]);
  printf("dump:0x05:%x\n", emu->registers[0x05]);
  printf("dump:0x06:%x\n", emu->registers[0x06]);
}

int main(int argc, char* argv[]){
  FILE* binary;
  Emulator* emu;

  if(argc != 2){
    printf("usage: px86 filename\n");
    return 1;
  }
  binary = fopen(argv[1], "rb");
  if(binary == NULL){
    printf("%sファイルが開けません\n", argv[1]);
    return 1;
  }
  emu = create_emu(MEMORY_SIZE, 0x7c00, 0x7c00);
  fread(emu->memory + 0x7c00, 1, 0x200, binary);
  fclose(binary);
  printf("memory:%s\n", emu->memory);

  init_instructions();
  dump_registers(emu);
  while(emu->eip < MEMORY_SIZE){
    uint8_t code = get_code8(emu, 0);
    printf("code:%x\n", code);
    if(instructions[code] == NULL){
      printf("\n\nNot Implemented: %x\n", code);
      break;
    }

    instructions[code](emu);
    printf("after emu->eip:%x\n", emu->eip);
    sleep(1);
    if(emu->eip == 0x00){
      printf("\n\nend of program.\n\n");
      dump_registers(emu);
      break;
    }
  }

  destroy_emu(emu);
  return 0;
}
