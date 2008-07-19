#include "NoMBC.h"
#include "Logger.h"

#include <iomanip>

u8 NoMBC::read(u16 addr) const
{
	if (addr <= 0x7FFF)
		return ROM[addr];
	else //if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
		return RAM[addr-0xA000];
	//else
	//	logger.error("NoMBC: Incorrect read");
	return 0;
}

u16 NoMBC::read16(u16 addr) const
{
	if (addr <= 0x7FFF)
		return ROM[addr] | (ROM[addr+1] << 8);
	else //if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
	{
		u16 offset = addr - 0xA000;
		return RAM[offset] | (RAM[offset+1] << 8);
	}
	//else
	//	logger.error("NoMBC: Incorrect read");
	return 0;
}

void NoMBC::write(u16 addr, u8 value)
{
	if ((addr&0xE000) == 0xA000) //(addr >= 0xA000 && addr <= 0xBFFF)
	{
		RAM[addr-0xA000]=value;
		return;
	}
	else
	{
		logger.debug("NoMBC: trying to write in ROM, addr=0x", std::hex, addr);
	}
}


