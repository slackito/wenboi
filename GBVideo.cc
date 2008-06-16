#include "GBVideo.h"

u8   GBVideo::read_VRAM (int addr) const
{
	return VRAM[addr-VRAM_BASE];
}

u8   GBVideo::read_OAM  (int addr) const
{
	return OAM[addr-OAM_BASE];
}

void GBVideo::write_VRAM(int addr, u8 value)
{
	VRAM[addr-VRAM_BASE] = value;
}

void GBVideo::write_OAM (int addr, u8 value)
{
	OAM[addr-OAM_BASE] = value;
}


