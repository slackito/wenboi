#include "MBC.h"
#include "Logger.h"
#include <cstring>

using namespace cartridge_types;

MBC *create_MBC(GBRom *rom)
{
	switch (rom->header.cartridge_type)
	{
		case ROM_ONLY:		return new NoMBC(rom);
		//case MBC1:		return new MBC1(rom);
		default:
			logger.critical("Unsupported cartridge type");
			return 0;
	}
}

u8 NoMBC::operator[](unsigned int addr) const
{
	if (addr <= 0x7FFF)
		return ROM[addr];
	else if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
		return RAM[addr-0xA000];
	else
		logger.error("NoMBC: Incorrect read");
	return 0;
}

u8& NoMBC::operator[](unsigned int addr) 
{
	if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
		return RAM[addr-0xA000];
	else
		logger.error("NoMBC: trying to write in ROM");
	return *(static_cast<u8*>(0)); // Shouldn't happen
}

