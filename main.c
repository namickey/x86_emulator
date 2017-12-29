#include <stdio.h>
#include <stdint.h>

#define REGISTERS_COUNT 16
#define ESP 1
#define MEMORY_SIZE 0x9c00
#define INST_SIZE 256

typedef struct {
  uint32_t registers[REGISTERS_COUNT];
  uint32_t eflags;
  uint8_t* memory;
  uint32_t eip;
} Emulator;

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

uint32_t get_code8(Emulator* emu, int index){
  return emu->memory[emu->eip + index];
}

uint32_t get_code32(Emulator* emu, int index){
  int i;
  uint32_t ret = 0;
  for(i = 0; i< 4; i++){
    ret |= get_code8(emu, index+i) << (i*8);
  }
  return ret;
}

int32_t get_sign_code8(Emulator* emu, int index){
  return (int8_t)emu->memory[emu->eip + index];
}

int32_t get_sign_code32(Emulator* emu, int index){
  return (int32_t)get_code32(emu, index);
}

void mov_r32_imm32(Emulator* emu){
  uint8_t reg = get_code8(emu, 0) - 0xB8;
  uint32_t value = get_code32(emu, 1);
  printf("mov_r32_imm32:reg:%x\n", reg);
  printf("mov_r32_imm32:value:%x\n", value);
  emu->registers[reg] = value;
  emu->eip += 5;
}

void short_jump(Emulator* emu){
  int8_t diff = get_sign_code8(emu, 1);
  printf("short_jump:diff:%x\n", diff);
  printf("short_jump:%x\n", emu->eip + (diff + 2));
  emu->eip += (diff + 2);
}

void near_jump(Emulator* emu){
  int32_t diff = get_sign_code32(emu, 1);
  printf("near_jump:diff:%x\n", diff);
  printf("near_jump:%x\n", emu->eip + (diff + 5));
  emu->eip += (diff + 5);
}

typedef void instruction_func_t(Emulator*);
instruction_func_t* instructions[INST_SIZE];
void init_instructions(void){
  int i;
  memset(instructions, 0, sizeof(instructions));
   for(i = 0; i < 8; i++){
    instructions[0xB8 + i] = mov_r32_imm32;
  }
  instructions[0xE9] = near_jump;
  instructions[0xEB] = short_jump;
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
    if(emu->eip == 0x00){
      printf("\n\nend of program.\n\n");
      dump_registers(emu);
      break;
    }
  }
  
  destroy_emu(emu);
  return 0;
}
