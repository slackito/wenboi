#include "GBVideo.h"
	
GBVideo::GBVideo(GameBoy *core):
	display(0),
	core(core)
{
	SDL_Init(SDL_INIT_VIDEO);
	display=SDL_SetVideoMode(160,144,32,SDL_SWSURFACE);

	colors[0] = SDL_MapRGB(display->format, 0xFF, 0xFF, 0xFF);
	colors[1] = SDL_MapRGB(display->format, 0xAA, 0xAA, 0xAA);
	colors[2] = SDL_MapRGB(display->format, 0x55, 0x55, 0x55);
	colors[3] = SDL_MapRGB(display->format, 0x00, 0x00, 0x00);
}

GBVideo::~GBVideo()
{
	SDL_Quit();
}

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

void GBVideo::update()
{
	//Mode 0 is present between 201-207 clks, 2 about 77-83 clks, and 3
	//about 169-175 clks. A complete cycle through these states takes 456
	//clks. VBlank lasts 4560 clks. A complete screen refresh occurs every
	//70224 clks.)
	//
	// sequence:
	// 2: 80 clocks           \
	// 3: 172 clocks          |-> for each one of the 144 lines
	// 0: 204 clocks (HBlank) /
	// 1: 4560 clocks -> VBlank
	
	int t = core->cycle_count % 70224;
	

}
