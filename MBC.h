#ifndef MBC_H
#define MBC_H

#include "sized_types.h"
#include "GBRom.h"
#include "logger.h"
#include <cstring>

class MBC
{
	public:
	virtual u8  operator[](unsigned int addr) const=0;
	virtual u8& operator[](unsigned int addr)=0;
	virtual ~MBC();
};

class NoMBC: public MBC
{
	u8 ROM[32768];
	u8 RAM[8192];
	
	public:
	NoMBC(GBRom *rom) { memcpy(ROM, rom->data, 32768); }

	u8 operator[](unsigned int addr) const
	{
		if (addr <= 0x7FFF)
			return ROM[addr];
		else if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
			return RAM[addr-0xA000];
		else
			logger.error("NoMBC: Incorrect read");
		return 0;
	}

	u8& operator[](unsigned int addr) 
	{
		if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
			return RAM[addr-0xA000];
		else
			logger.error("NoMBC: trying to write in ROM");
		return *(static_cast<u8*>(0)); // Shouldn't happen
	}
};


MBC *create_MBC(GBRom *rom);

#endif // MBC_H
