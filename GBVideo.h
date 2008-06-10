#include "GBMemory.h"

class GBVideo
{
	GameBoy *core;

	u8 VRAM[8192];
	u8 OAM[];

	public:
	GBVideo(GameBoy *core):core(core) {}

	u8   read_VRAM (int addr) const;
	u8   read_OAM  (int addr) const;
	void write_VRAM(int addr, u8 value);
	void write_OAM (int addr, u8 value);
};


