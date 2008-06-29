#include "GBVideo.h"
#include "gbcore.h"
#include "util.h"

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
	int STAT = memory.read(GBIO::STAT);
	if ((STAT & 3) == 3)
		return 0xFF; // VRAM access disabled
	else
		return VRAM[addr-VRAM_BASE];
}

u8   GBVideo::read_OAM  (int addr) const
{
	int STAT = memory.read(GBIO::STAT);
	if ((STAT & 3) >= 2)
		return 0xFF; // OAM access disabled
	else
		return OAM[addr-OAM_BASE];
}

void GBVideo::write_VRAM(int addr, u8 value)
{
	int STAT = memory.read(GBIO::STAT);
	if ((STAT & 3) == 3)
		return; // VRAM access disabled
	else
		VRAM[addr-VRAM_BASE] = value;
}

void GBVideo::write_OAM (int addr, u8 value)
{
	int STAT = memory.read(GBIO::STAT);
	if ((STAT & 3) >= 2)
		return; // OAM access disabled
	else
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
	// 2: 80 clocks  (reading OAM)       |
	// 3: 172 clocks (reading OAM+VRAM)  |-> for each one of the 144 lines
	// 0: 204 clocks (HBlank)            |
	// 1: 4560 clocks -> VBlank
	//
	// mode 2 starts at 0, 456, 912...
	// mode 3 starts at 80, 536, 992...
	// mode 0 starts at 252, 708, 1164...
	// vblank starts at 65664

	int STAT = core->memory.read(GBIO::STAT);
	int LYC  = core->memory.read(GBIO::LYC);
	int LY   = core->memory.read(GBIO::LY);

	int t = core->cycle_count % 70224;
	
	if (t >= 65665)
	{
		if (t == 65665 && check_bit(STAT,4))
			core->irq(GameBoy::IRQ_VBLANK);

		// preserve bits 3-6, set mode to 1 (VBlank) and coincidence to 0
		STAT = (STAT&0xF8) | 1;
	}
	else
	{
		LY = t/456;
		int hline_t = t%456;
		if (LY == LYC)
		{
			STAT = set_bit(STAT, 2); // set coincidence flag
			if (hline_t == 0 && check_bit(STAT, 6))
				core->irq(GameBoy::IRQ_LCD_STAT);
		}

		if (hline_t < 80)
		{
			if (hline_t == 0 && check_bit(STAT, 5))
				core->irq(GameBoy::IRQ_LCD_STAT);

			// preserve bits 2-6, set mode 2
			STAT = (STAT&0xFC) | 2;
		}
		else if (hline_t < 252)
		{
			// preserve bits 2-6, set mode 3
			STAT = (STAT&0xFC) | 3;
		}
		else
		{
			// HBlank (preserve bits 2-6, mode = 0)
			STAT = (STAT&0xFC);
			if (hline_t == 252 && check_bit(STAT, 3))
				core->irq(GameBoy::IRQ_LCD_STAT);
		}
		
	}
	
	core->memory.write(GBIO::LY, LY);
	core->memory.write(GBIO::STAT, STAT);

	// Draw the background
	

}
