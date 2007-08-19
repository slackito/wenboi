#include "GBMemory.h"
#include "MBC.h"
#include "gbcore.h"
#include "Logger.h"

u8& GBMemory::operator[](unsigned int addr)
{
	if (addr < 0x8000)      return (*mbc)[addr];
	else if (addr < 0xA000) return VRAM[addr-0x8000];
	else if (addr < 0xC000) return (*mbc)[addr];
	else if (addr < 0xD000) return WRAM0[addr-0xC000];
	else if (addr < 0xE000) return WRAM1[addr-0xD000];
	else if (addr < 0xFDFF) return (*mbc)[addr-0x2000];
	else if (addr < 0xFEA0) return OAM[addr-0xFDFF];
	else if (addr >= 0xFF00 && addr <= 0xFF7F) 
		return core->IO.addr[addr-0xFF00];
	else if (addr >= 0xFF80 && addr <= 0xFFFE) return HRAM[addr-0xFF80];
	else {
		logger.error("Invalid write address");
		return *(static_cast<u8*>(0));
	}
}


u8  GBMemory::operator[](unsigned int addr) const
{
	if (addr < 0x8000)      return (*mbc)[addr];
	else if (addr < 0xA000) return VRAM[addr-0x8000];
	else if (addr < 0xC000) return (*mbc)[addr];
	else if (addr < 0xD000) return WRAM0[addr-0xC000];
	else if (addr < 0xE000) return WRAM1[addr-0xD000];
	else if (addr < 0xFDFF) return (*mbc)[addr-0x2000];
	else if (addr < 0xFEA0) return OAM[addr-0xFDFF];
	else if (addr >= 0xFF00 && addr <= 0xFF7F) 
		return core->IO.addr[addr-0xFF00];
	else if (addr >= 0xFF80 && addr <= 0xFFFE) return HRAM[addr-0xFF80];
	else {
		logger.error("Invalid write address");
		return *(static_cast<u8*>(0));
	}
}



