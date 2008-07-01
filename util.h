#ifndef UTIL_H
#define UTIL_H

#include "sized_types.h"

uint32 set_bit(uint32 val, uint32 pos);
uint32 reset_bit(uint32 val, uint32 pos);
uint32 flip_bit(uint32 val, uint32 pos);
bool check_bit(uint32 val, uint32 pos);

#endif

