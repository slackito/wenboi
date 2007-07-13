#ifndef GBMEMORY_H
#define GBMEMORY_H

#include "sized_types.h"
#include "MBC.h"

class GBMemory
{
	MBC *mbc;
	                // 0000-3FFF: ROM Bank 0 (in cart)
			// 4000-7FFF: Switchable ROM Bank (in cart)
	u8 VRAM[8192];  // 8000-9FFF: Video RAM
			// A000-BFFF: External RAM (in cart, switchable)
	u8 WRAM0[4096]; // C000-CFFF: Work RAM Bank 0
	u8 WRAM1[4096]; // D000-DFFF: Work RAM Bank 1 (TODO: In GBC mode switchable bank 1-7)
			// E000-FDFF: ECHO: Same as C000-DDFF
	u8 OAM[160];    // FE00-FE9F: Sprite Attribute Table
	u8 HRAM[126];   // FF80-FFFE: High RAM

	public:
	GBMemory(): mbc(0) {}
	void init(MBC *mbc) { this->mbc = mbc; }


	int& operator[](int index)=0;
	int  operator[](int index) const=0;

};


#endif // GBMEMORY_H
