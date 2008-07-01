#include "GBMemory.h"
#include "MBC.h"
#include "gbcore.h"
#include "Logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
	
u8   GBIO::read(int addr) const
{
	return ports[addr-IO_BASE];
}

void GBIO::write(int addr, u8 value)
{
	ports[addr-IO_BASE] = value;
}

void GBMemory::write(int addr, u8 value)
{
	if (addr < 0x8000)      mbc->write(addr, value);
	else if (addr < 0xA000) core->video.write_VRAM(addr, value);
	else if (addr < 0xC000) mbc->write(addr, value);
	else if (addr < 0xE000) WRAM[addr - WRAM_BASE] = value;
	else if (addr < 0xFE00) write(addr-0x2000, value);
	else if (addr < 0xFEA0) core->video.write_OAM (addr, value);
	else if (addr >= 0xFF00 && addr <= 0xFF7F) IO.write(addr, value);
	else if (addr >= 0xFF80 && addr <= 0xFFFE) HRAM[addr - HRAM_BASE]=value;
	else if (addr == 0xFFFF) IE=value;
	else {
		std::ostringstream errmsg;
		errmsg << "Invalid write address 0x" << 
			std::hex << std::setw(4) << std::setfill('0') << addr;
		logger.debug(errmsg.str());
		//std::cout << *(static_cast<u8*>(0));
	}
}


u8  GBMemory::read(int addr) const
{
	if (addr < 0x8000)      return mbc->read(addr);
	else if (addr < 0xA000) return core->video.read_VRAM(addr);
	else if (addr < 0xC000) return mbc->read(addr);
	else if (addr < 0xE000) return WRAM[addr - WRAM_BASE];
	else if (addr < 0xFDFF) return read(addr-0x2000);
	else if (addr < 0xFEA0) return core->video.read_OAM (addr);
	else if (addr >= 0xFF00 && addr <= 0xFF7F) return IO.read(addr);
	else if (addr >= 0xFF80 && addr <= 0xFFFE) return HRAM[addr - HRAM_BASE];
	else if (addr == 0xFFFF) return IE;
	else {
		std::ostringstream errmsg;
		errmsg << "Invalid read address 0x" << 
			std::hex << std::setw(4) << std::setfill('0') << addr;
		logger.error(errmsg.str());
		return *(static_cast<u8*>(0));
	}
}



