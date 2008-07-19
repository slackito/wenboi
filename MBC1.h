#ifndef MBC1_H
#define MBC1_H


#include "MBC.h"


class MBC1: public MBC
{
	u8 ROM[128*16384];
	u8 RAM[  4* 8192];

	u8 rom_bank_low;
	u8 ram_bank;
	bool ram_enabled;

	enum BankingMode
	{
		ROM_BANKING_MODE=0,
		RAM_BANKING_MODE=1
	} mode;

	public:
	MBC1(GBRom *rom): rom_bank_low(1), ram_bank(0), ram_enabled(false), mode(ROM_BANKING_MODE)
	{
		memcpy(ROM, rom->data, 32768 << rom->header.rom_size);
	}

	u8   read  (u16 addr) const;
	u16  read16(u16 addr) const;
	void write (u16 addr, u8 value);
};

#endif

