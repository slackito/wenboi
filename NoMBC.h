#ifndef NOMBC_H
#define NOMBC_H

#include "MBC.h"

class NoMBC: public MBC
{
	u8 ROM[32768];
	u8 RAM[8192];

	public:
	NoMBC(GBRom *rom) { memcpy(ROM, rom->data, 32768); }
	u8   read  (u16 addr) const;
	u16  read16(u16 addr) const;
	void write (u16 addr, u8 value);

};

#endif
