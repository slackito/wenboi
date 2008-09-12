#include "GBMemory.h"
#include "MBC.h"
#include "gbcore.h"
#include "Logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>


int GBMemory::set_watchpoint(u16 addr)
{
	watchpoints[++last_watchpoint_id] = Watchpoint(addr, true);
	return last_watchpoint_id;
}

void GBMemory::delete_watchpoint(int id)
{
	watchpoints.erase(id);
}

void GBMemory::enable_watchpoint(int id)
{
	watchpoints[id].enabled = true;
}

void GBMemory::disable_watchpoint(int id)
{
	watchpoints[id].enabled = false;
}

void GBMemory::check_watchpoints(u16 addr, u16 value)
{
	for (WatchpointMap::iterator i = watchpoints.begin();
			i != watchpoints.end(); i++)
	{
		if (i->second.enabled && addr == i->second.addr)
		{
			watchpoint_reached = true;
			watchpoint_addr = addr;
			watchpoint_oldvalue = read(addr, DONT_WATCH);
			watchpoint_newvalue = value;
			break;
		}
	}
}
	
void GBMemory::write(u16 addr, u8 value, WatchpointControl watch)
{
	if (watch == WATCH)
		check_watchpoints(addr, value);

	if (addr < 0x8000)      mbc->write(addr, value);
	else if (addr < 0xA000) core->video.write_VRAM(addr, value);
	else if (addr < 0xC000) mbc->write(addr, value);
	else if (addr < 0xE000) WRAM[addr - WRAM_BASE] = value;
	else if (addr < 0xFE00) write(addr-0x2000, value);
	else if (addr < 0xFEA0) core->video.write_OAM (addr, value);
	else if (addr >= 0xFF00) {
		high[addr-0xFF00] = value;
		if (addr == DIV) 
		{
			high[I_DIV] = 0;
		}
		else if (addr == DMA)
		{
			u16 dma_src = value << 8;
			//logger.warning("OAM DMA transfer from 0x", std::hex, std::setfill('0'), dma_src, " requested");
			core->video.DMA_OAM(dma_src);
		}
	}
	else {
		std::ostringstream errmsg;
		errmsg << "Invalid write address 0x" << 
			std::hex << std::setw(4) << std::setfill('0') << addr;
		logger.debug(errmsg.str());
		//std::cout << *(static_cast<u8*>(0));
	}
}


u8  GBMemory::read(u16 addr, WatchpointControl watch)
{
	if (watch == WATCH)
		check_watchpoints(addr, 0xFFFF);

	if (addr < 0x8000)      return mbc->read(addr);
	else if (addr < 0xA000) return core->video.read_VRAM(addr);
	else if (addr < 0xC000) return mbc->read(addr);
	else if (addr < 0xE000) return WRAM[addr - WRAM_BASE];
	else if (addr < 0xFDFF) return read(addr-0x2000);
	else if (addr < 0xFEA0) return core->video.read_OAM (addr);
	else if (addr >= 0xFF00) return high[addr-0xFF00];
	else {
		std::ostringstream errmsg;
		errmsg << "Invalid read address 0x" << 
			std::hex << std::setw(4) << std::setfill('0') << addr;
		logger.error(errmsg.str());
		//return *(static_cast<u8*>(0));
		return 0;
	}
}

u16 GBMemory::read16(u16 addr, WatchpointControl watch)
{
	if (watch == WATCH)
		check_watchpoints(addr, 0xFFFF);

	if (addr < 0x8000)      return mbc->read16(addr);
	else if (addr < 0xA000) return core->video.read16_VRAM(addr);
	else if (addr < 0xC000) return mbc->read16(addr);
	else if (addr < 0xE000) return WRAM[addr - WRAM_BASE] + (WRAM[addr - WRAM_BASE + 1] << 8);
	else if (addr < 0xFDFF) return read16(addr-0x2000);
	else if (addr < 0xFEA0) return core->video.read16_OAM (addr);
	else if (addr >= 0xFF00) return high[addr-0xFF00] + (high[addr-0xFF00+1] << 8);
	else {
		std::ostringstream errmsg;
		errmsg << "Invalid read address 0x" << 
			std::hex << std::setw(4) << std::setfill('0') << addr;
		logger.error(errmsg.str());
		return *(static_cast<u8*>(0));
	}
}

