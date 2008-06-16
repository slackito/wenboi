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
	static const u16 LCDC = 0xFF40; // LCD Control          (R/W)
	static const u16 STAT = 0xFF41; // LCD Status           (R/W)
	static const u16 SCY  = 0xFF42; // Scroll Y             (R/W)
	static const u16 SCX  = 0xFF43; // Scroll X             (R/W)
	static const u16 LY   = 0xFF44; // LCDC Y coord         (R)
	static const u16 LYC  = 0xFF45; // LY Compare           (R/W)
	static const u16 WY   = 0xFF4A; // Window Y pos         (R/W)
	static const u16 WX   = 0xFF4B; // Window X pos minus 7 (R/W)
	static const u16 BGP  = 0xFF47; // BG Pallette data     (R/W)
	static const u16 OBP0 = 0xFF48; // Object Pallete 0 data(R/W)
	static const u16 OBP1 = 0xFF49; // Object Pallete 1 data(R/W)
	static const u16 DMA  = 0xFF46; // DMA Transfer & Start addr (W)

	u8   read(int addr) const;
	void write(int addr, u8 value);

};

class GBMemory
{
	GameBoy *core;
	MBC *mbc;
	                // 0000-3FFF: ROM Bank 0 (in cart)
			// 4000-7FFF: Switchable ROM Bank (in cart)
			// 8000-9FFF: Video RAM
			// A000-BFFF: External RAM (in cart, switchable)
	u8 WRAM0[4096]; // C000-CFFF: Work RAM Bank 0
	u8 WRAM1[4096]; // D000-DFFF: Work RAM Bank 1 (TODO: In GBC mode switchable bank 1-7)
			// E000-FDFF: ECHO: Same as C000-DDFF
			// FE00-FE9F: Sprite Attribute Table (OAM)
			// FEA0-FEFF: Not usable
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
