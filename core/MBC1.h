/*
    Copyright 2008 Jorge Gorbe Moya <slack@codemaniacs.com>

    This file is part of wenboi 

    wenboi is free software: you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 3 only, as published by the
    Free Software Foundation.

    wenboi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wenboi.  If not, see <http://www.gnu.org/licenses/>.
*/ 
#ifndef MBC1_H
#define MBC1_H

#include "MBC.h"
#include <cstring>


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

        u32 getUniqueAddress(u16 addr) const;
};

#endif

