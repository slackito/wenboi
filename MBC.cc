#include "MBC.h"
#include "Logger.h"
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>

using namespace cartridge_types;

MBC *create_MBC(GBRom *rom)
{
	switch (rom->header.cartridge_type)
	{
		case ROM_ONLY:		return new NoMBC(rom);
		//case MBC1:		return new MBC1(rom);
		default:
			logger.critical("Unsupported cartridge type ", 
					int(rom->header.cartridge_type));
			return 0;
	}
}

u8 NoMBC::read(int addr) const
{
	if (addr <= 0x7FFF)
		return ROM[addr];
	else //if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
		return RAM[addr-0xA000];
	//else
	//	logger.error("NoMBC: Incorrect read");
	return 0;
}

u16 NoMBC::read16(int addr) const
{
	if (addr <= 0x7FFF)
		return ROM[addr]+(ROM[addr+1] << 8);
	else //if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
		return RAM[addr-0xA000] + (RAM[addr-0xA000+1] << 8);
	//else
	//	logger.error("NoMBC: Incorrect read");
	return 0;
}

void NoMBC::write(int addr, u8 value)
{
	if ((addr&0xE000) == 0xA000) //(addr >= 0xA000 && addr <= 0xBFFF)
	{
		RAM[addr-0xA000]=value;
		return;
	}
	else 
	{
		std::ostringstream errmsg;
		errmsg <<"NoMBC: trying to write in ROM, addr=0x"<<std::hex<<addr;
		logger.debug(errmsg.str());
	}
}

