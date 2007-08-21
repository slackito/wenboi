#ifndef GBMEMORY_H
#define GBMEMORY_H

#include "sized_types.h"

class GameBoy;
class MBC;

class GBIO
{
	u8 ports[128];

	public:
	static const u16 IO_BASE = 0xFF00;

	u8   read(int addr) const { return ports[addr-IO_BASE]; }
	void write(int addr, u8 value) { ports[addr-IO_BASE] = value; }

};

class GBMemory
{
	GameBoy *core;
	MBC *mbc;
	                // 0000-3FFF: ROM Bank 0 (in cart)
			// 4000-7FFF: Switchable ROM Bank (in cart)
	u8 VRAM[8192];  // 8000-9FFF: Video RAM
			// A000-BFFF: External RAM (in cart, switchable)
	u8 WRAM0[4096]; // C000-CFFF: Work RAM Bank 0
	u8 WRAM1[4096]; // D000-DFFF: Work RAM Bank 1 (TODO: In GBC mode switchable bank 1-7)
			// E000-FDFF: ECHO: Same as C000-DDFF
	u8 OAM[160];    // FE00-FE9F: Sprite Attribute Table
	GBIO IO;	// FF00-FF7F: IO ports

	u8 HRAM[126];   // FF80-FFFE: High RAM
	u8 IE;		// FFFF     : Interrupt Enable

	public:

	static const u16 VRAM_BASE  = 0x8000;
	static const u16 EXTERNAL_RAM_BASE = 0xA000;
	static const u16 WRAM0_BASE = 0xC000;
	static const u16 WRAM1_BASE = 0xD000;
	static const u16 OAM_BASE   = 0xFE00;
	static const u16 IO_BASE    = 0xFF00;
	static const u16 HRAM_BASE  = 0xFF80;

	GBMemory(GameBoy *core): core(core), mbc(0), IO(), IE(0) {}
	void init(MBC *mbc) { this->mbc = mbc; }


	u8   read(int addr) const;
	void write(int addr, u8 value);

};


#endif // GBMEMORY_H
