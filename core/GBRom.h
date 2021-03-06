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
#ifndef GBROM_H
#define GBROM_H

#include "../common/sized_types.h"
#include <string>

namespace cartridge_types {
	const u8 ROM_ONLY=0x00;
	const u8 MBC1=0x01;
	const u8 MBC1_RAM=0x02;
	const u8 MBC1_RAM_BATTERY=0x03;
	const u8 MBC2=0x05;
	const u8 MBC2_BATTERY=0x06;
	const u8 ROM_RAM=0x08;
	const u8 ROM_RAM_BATTERY=0x09;
	const u8 MMM01=0x0B;
	const u8 MMM01_RAM=0x0C;
	const u8 MMM01_RAM_BATTERY=0x0D;
	const u8 MBC3_TIMER_BATTERY=0X0F;
	const u8 MBC3_TIMER_RAM_BATTERY=0X10;
	const u8 MBC3=0X11;
	const u8 MBC3_RAM=0X12;
	const u8 MBC3_RAM_BATTERY=0X13;
	const u8 MBC4=0X15;
	const u8 MBC4_RAM=0X16;
	const u8 MBC4_RAM_BATTERY=0X17;
	const u8 MBC5=0X19;
	const u8 MBC5_RAM=0X1A;
	const u8 MBC5_RAM_BATTERY=0X1B;
	const u8 MBC5_RUMBLE=0X1C;
	const u8 MBC5_RUMBLE_RAM=0X1D;
	const u8 MBC5_RUMBLE_RAM_BATTERY=0X1E;
	const u8 POCKET_CAMERA=0XFC;
	const u8 BANDAI_TAMA5=0XFD;
	const u8 HuC3=0XFE;
	const u8 HuC1_RAM_BATTERY=0xFF;
}




struct GBRomHeader {
	u8  dummy[256];            // ROM beginning, before header
	u8  entry_point[4];        // 0100-0103
	u8  nintendo_logo[48];     // 0104-0133
	union {
		struct {
			u8 old_title[16];
		};
		struct {
			u8 new_title[11];
			u8 manuf_code[4];
			u8 cgb_flag;
		};
	};
	u8  new_licensee_code[2];
	u8  sgb_flag;
	u8  cartridge_type;
	u8  rom_size; // 32Kb shl rom_size
	u8  ram_size;
	u8  destination_code;
	u8  old_licensee_code;
	u8  mask_rom_version_number;
	u8  header_checksum;
	u8  global_checksum[2];
};

union GBRom {
	GBRomHeader header;
	u8 data[1]; // struct hack
};

GBRom *read_gbrom(std::string filename);


#endif

