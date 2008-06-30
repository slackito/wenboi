#ifndef UTIL_H
#define UTIL_H

#include "sized_types.h"

uint32 set_bit(uint32 val, uint32 pos)
{
	uint32 mask = 1<<pos;
	return val | mask;
}

uint32 reset_bit(uint32 val, uint32 pos)
{
	uint32 mask = ~(1<<pos);
	return val & mask;
}

uint32 flip_bit(uint32 val, uint32 pos)
{
	uint32 mask = 1<<pos;
	return val ^ mask;
}

bool check_bit(uint32 val, uint32 pos)
{
	uint32 mask = 1<<pos;
	return ((val&mask) != 0);
}

#endif

