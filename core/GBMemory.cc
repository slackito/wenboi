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
#include "GBMemory.h"
#include "GameBoy.h"
#include "MBC.h"
#include "../common/Logger.h"
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
	else if (addr == 0xFF00) {
		high[0] = (high[0] & 0xCF) | (value & 0x30);
		core->update_JOYP();
	}
	else if (addr > 0xFF00) {
		int port_number = addr - 0xFF00;
		// Check if port is writable
		if (port_access[port_number] != READ_ONLY)
		{
			// Do the write
			high[port_number] = value;
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
		else
		{
			logger.warning("WTF? Trying to write to addr=0x",std::hex, addr, " (read-only port). PC=0x",std::hex,core->regs.PC);
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
	else if (addr >= 0xFF00) 
	{
		int port_number=addr-0xFF00;
		// Check access
		if (port_access[port_number] == WRITE_ONLY)
		{
			logger.warning("WTF? Trying to read from addr=0x",std::hex, addr, " (write-only port). PC=0x",std::hex,core->regs.PC);
		}
		return high[port_number];
	}
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

const GBMemory::PortAccess GBMemory::port_access[256]=
{
	READWRITE, // 0xFF00
	READWRITE, // 0xFF01
	READWRITE, // 0xFF02
	READWRITE, // 0xFF03
	READWRITE, // 0xFF04
	READWRITE, // 0xFF05
	READWRITE, // 0xFF06
	READWRITE, // 0xFF07
	READWRITE, // 0xFF08
	READWRITE, // 0xFF09
	READWRITE, // 0xFF0A
	READWRITE, // 0xFF0B
	READWRITE, // 0xFF0C
	READWRITE, // 0xFF0D
	READWRITE, // 0xFF0E
	READWRITE, // 0xFF0F
	READWRITE, // 0xFF10
	READWRITE, // 0xFF11
	READWRITE, // 0xFF12
	READWRITE, // 0xFF13
	READWRITE, // 0xFF14
	READWRITE, // 0xFF15
	READWRITE, // 0xFF16
	READWRITE, // 0xFF17
	READWRITE, // 0xFF18
	READWRITE, // 0xFF19
	READWRITE, // 0xFF1A
	READWRITE, // 0xFF1B
	READWRITE, // 0xFF1C
	READWRITE, // 0xFF1D
	READWRITE, // 0xFF1E
	READWRITE, // 0xFF1F
	READWRITE, // 0xFF20
	READWRITE, // 0xFF21
	READWRITE, // 0xFF22
	READWRITE, // 0xFF23
	READWRITE, // 0xFF24
	READWRITE, // 0xFF25
	READWRITE, // 0xFF26
	READWRITE, // 0xFF27
	READWRITE, // 0xFF28
	READWRITE, // 0xFF29
	READWRITE, // 0xFF2A
	READWRITE, // 0xFF2B
	READWRITE, // 0xFF2C
	READWRITE, // 0xFF2D
	READWRITE, // 0xFF2E
	READWRITE, // 0xFF2F
	READWRITE, // 0xFF30
	READWRITE, // 0xFF31
	READWRITE, // 0xFF32
	READWRITE, // 0xFF33
	READWRITE, // 0xFF34
	READWRITE, // 0xFF35
	READWRITE, // 0xFF36
	READWRITE, // 0xFF37
	READWRITE, // 0xFF38
	READWRITE, // 0xFF39
	READWRITE, // 0xFF3A
	READWRITE, // 0xFF3B
	READWRITE, // 0xFF3C
	READWRITE, // 0xFF3D
	READWRITE, // 0xFF3E
	READWRITE, // 0xFF3F
	READWRITE, // 0xFF40
	READWRITE, // 0xFF41
	READWRITE, // 0xFF42
	READWRITE, // 0xFF43
	READ_ONLY, // 0xFF44
	READWRITE, // 0xFF45
	WRITE_ONLY, // 0xFF46
	READWRITE, // 0xFF47
	READWRITE, // 0xFF48
	READWRITE, // 0xFF49
	READWRITE, // 0xFF4A
	READWRITE, // 0xFF4B
	READWRITE, // 0xFF4C
	READWRITE, // 0xFF4D
	READWRITE, // 0xFF4E
	READWRITE, // 0xFF4F
	READWRITE, // 0xFF50
	READWRITE, // 0xFF51
	READWRITE, // 0xFF52
	READWRITE, // 0xFF53
	READWRITE, // 0xFF54
	READWRITE, // 0xFF55
	READWRITE, // 0xFF56
	READWRITE, // 0xFF57
	READWRITE, // 0xFF58
	READWRITE, // 0xFF59
	READWRITE, // 0xFF5A
	READWRITE, // 0xFF5B
	READWRITE, // 0xFF5C
	READWRITE, // 0xFF5D
	READWRITE, // 0xFF5E
	READWRITE, // 0xFF5F
	READWRITE, // 0xFF60
	READWRITE, // 0xFF61
	READWRITE, // 0xFF62
	READWRITE, // 0xFF63
	READWRITE, // 0xFF64
	READWRITE, // 0xFF65
	READWRITE, // 0xFF66
	READWRITE, // 0xFF67
	READWRITE, // 0xFF68
	READWRITE, // 0xFF69
	READWRITE, // 0xFF6A
	READWRITE, // 0xFF6B
	READWRITE, // 0xFF6C
	READWRITE, // 0xFF6D
	READWRITE, // 0xFF6E
	READWRITE, // 0xFF6F
	READWRITE, // 0xFF70
	READWRITE, // 0xFF71
	READWRITE, // 0xFF72
	READWRITE, // 0xFF73
	READWRITE, // 0xFF74
	READWRITE, // 0xFF75
	READWRITE, // 0xFF76
	READWRITE, // 0xFF77
	READWRITE, // 0xFF78
	READWRITE, // 0xFF79
	READWRITE, // 0xFF7A
	READWRITE, // 0xFF7B
	READWRITE, // 0xFF7C
	READWRITE, // 0xFF7D
	READWRITE, // 0xFF7E
	READWRITE, // 0xFF7F
	READWRITE, // 0xFF80
	READWRITE, // 0xFF81
	READWRITE, // 0xFF82
	READWRITE, // 0xFF83
	READWRITE, // 0xFF84
	READWRITE, // 0xFF85
	READWRITE, // 0xFF86
	READWRITE, // 0xFF87
	READWRITE, // 0xFF88
	READWRITE, // 0xFF89
	READWRITE, // 0xFF8A
	READWRITE, // 0xFF8B
	READWRITE, // 0xFF8C
	READWRITE, // 0xFF8D
	READWRITE, // 0xFF8E
	READWRITE, // 0xFF8F
	READWRITE, // 0xFF90
	READWRITE, // 0xFF91
	READWRITE, // 0xFF92
	READWRITE, // 0xFF93
	READWRITE, // 0xFF94
	READWRITE, // 0xFF95
	READWRITE, // 0xFF96
	READWRITE, // 0xFF97
	READWRITE, // 0xFF98
	READWRITE, // 0xFF99
	READWRITE, // 0xFF9A
	READWRITE, // 0xFF9B
	READWRITE, // 0xFF9C
	READWRITE, // 0xFF9D
	READWRITE, // 0xFF9E
	READWRITE, // 0xFF9F
	READWRITE, // 0xFFA0
	READWRITE, // 0xFFA1
	READWRITE, // 0xFFA2
	READWRITE, // 0xFFA3
	READWRITE, // 0xFFA4
	READWRITE, // 0xFFA5
	READWRITE, // 0xFFA6
	READWRITE, // 0xFFA7
	READWRITE, // 0xFFA8
	READWRITE, // 0xFFA9
	READWRITE, // 0xFFAA
	READWRITE, // 0xFFAB
	READWRITE, // 0xFFAC
	READWRITE, // 0xFFAD
	READWRITE, // 0xFFAE
	READWRITE, // 0xFFAF
	READWRITE, // 0xFFB0
	READWRITE, // 0xFFB1
	READWRITE, // 0xFFB2
	READWRITE, // 0xFFB3
	READWRITE, // 0xFFB4
	READWRITE, // 0xFFB5
	READWRITE, // 0xFFB6
	READWRITE, // 0xFFB7
	READWRITE, // 0xFFB8
	READWRITE, // 0xFFB9
	READWRITE, // 0xFFBA
	READWRITE, // 0xFFBB
	READWRITE, // 0xFFBC
	READWRITE, // 0xFFBD
	READWRITE, // 0xFFBE
	READWRITE, // 0xFFBF
	READWRITE, // 0xFFC0
	READWRITE, // 0xFFC1
	READWRITE, // 0xFFC2
	READWRITE, // 0xFFC3
	READWRITE, // 0xFFC4
	READWRITE, // 0xFFC5
	READWRITE, // 0xFFC6
	READWRITE, // 0xFFC7
	READWRITE, // 0xFFC8
	READWRITE, // 0xFFC9
	READWRITE, // 0xFFCA
	READWRITE, // 0xFFCB
	READWRITE, // 0xFFCC
	READWRITE, // 0xFFCD
	READWRITE, // 0xFFCE
	READWRITE, // 0xFFCF
	READWRITE, // 0xFFD0
	READWRITE, // 0xFFD1
	READWRITE, // 0xFFD2
	READWRITE, // 0xFFD3
	READWRITE, // 0xFFD4
	READWRITE, // 0xFFD5
	READWRITE, // 0xFFD6
	READWRITE, // 0xFFD7
	READWRITE, // 0xFFD8
	READWRITE, // 0xFFD9
	READWRITE, // 0xFFDA
	READWRITE, // 0xFFDB
	READWRITE, // 0xFFDC
	READWRITE, // 0xFFDD
	READWRITE, // 0xFFDE
	READWRITE, // 0xFFDF
	READWRITE, // 0xFFE0
	READWRITE, // 0xFFE1
	READWRITE, // 0xFFE2
	READWRITE, // 0xFFE3
	READWRITE, // 0xFFE4
	READWRITE, // 0xFFE5
	READWRITE, // 0xFFE6
	READWRITE, // 0xFFE7
	READWRITE, // 0xFFE8
	READWRITE, // 0xFFE9
	READWRITE, // 0xFFEA
	READWRITE, // 0xFFEB
	READWRITE, // 0xFFEC
	READWRITE, // 0xFFED
	READWRITE, // 0xFFEE
	READWRITE, // 0xFFEF
	READWRITE, // 0xFFF0
	READWRITE, // 0xFFF1
	READWRITE, // 0xFFF2
	READWRITE, // 0xFFF3
	READWRITE, // 0xFFF4
	READWRITE, // 0xFFF5
	READWRITE, // 0xFFF6
	READWRITE, // 0xFFF7
	READWRITE, // 0xFFF8
	READWRITE, // 0xFFF9
	READWRITE, // 0xFFFA
	READWRITE, // 0xFFFB
	READWRITE, // 0xFFFC
	READWRITE, // 0xFFFD
	READWRITE, // 0xFFFE
	READWRITE, // 0xFFFF
};

