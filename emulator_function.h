#ifndef EMULATOR_FUNCTION_H_
#define EMULATOR_FUNCTION_H_

#include "emulator.h"

uint32_t get_code8(Emulator* emu, int index);
uint32_t get_code32(Emulator* emu, int index);
int32_t get_sign_code8(Emulator* emu, int index);
int32_t get_sign_code32(Emulator* emu, int index);

#endif
