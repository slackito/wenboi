#include "GBVideo.h"
#include "gbcore.h"
#include "util.h"
#include <iostream>


GBVideo::GBVideo(GameBoy *core):
	display(0),
	core(core),
	frames_rendered(0)
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
	int STAT = core->memory.read(GBIO::STAT);
	if ((STAT & 3) == 3)
		return 0xFF; // VRAM access disabled
	else
		return VRAM[addr-VRAM_BASE];
}

u8   GBVideo::read_OAM  (int addr) const
{
	int STAT = core->memory.read(GBIO::STAT);
	if ((STAT & 3) >= 2)
		return 0xFF; // OAM access disabled
	else
		return OAM[addr-OAM_BASE];
}

void GBVideo::write_VRAM(int addr, u8 value)
{
	int STAT = core->memory.read(GBIO::STAT);
	if ((STAT & 3) == 3)
		return; // VRAM access disabled
	else
		VRAM[addr-VRAM_BASE] = value;
}

void GBVideo::write_OAM (int addr, u8 value)
{
	int STAT = core->memory.read(GBIO::STAT);
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

	u32 *pixels = static_cast<u32*>(display->pixels);
	u32 pixels_per_line = display->pitch/display->format->BytesPerPixel;

	int STAT = core->memory.read(GBIO::STAT);
	int LYC  = core->memory.read(GBIO::LYC);

	int t = core->cycle_count % 70224;
	int hline_t=-1;
	int	LY = t/456;
	//std::cout << t << std::endl;

	if (t >= 65664)
	{
		if (t == 65664)
		{
			if (check_bit(STAT,4))
				core->irq(GameBoy::IRQ_VBLANK);
			SDL_UpdateRect(display, 0, 0, 0, 0);
			frames_rendered++;
			char buf[50];
			sprintf(buf, "%d", frames_rendered);
			SDL_WM_SetCaption(buf, 0);
		}

		// preserve bits 3-6, set mode to 1 (VBlank) and coincidence to 0
		STAT = (STAT&0xF8) | 1;
	}
	else
	{
		hline_t = t%456;
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
	// Draw at hline_t == 80, when the app cannot write to neither VRAM nor OAM
	if (hline_t == 80)
	{
		int LCDC = core->memory.read(GBIO::LCDC);
		int BGP  = core->memory.read(GBIO::BGP);
		int pallette[4];
		pallette[0] = BGP & 3;
		pallette[1] = (BGP>>2) & 3;
		pallette[2] = (BGP>>4) & 3;
		pallette[3] = (BGP>>6) & 3;
		
		if (check_bit(LCDC, 0))  // is BG display active?
		{
			u16 tile_map_addr  = check_bit(LCDC,3) ? 0x1C00  : 0x1800;
			u16 tile_data_addr = check_bit(LCDC,4) ? 0x0800 : 0x0000;
			int tile_data_base = (tile_data_addr == 0x0800) ? -128 : 127;

			// (vx    , vy    ) -> position of the pixel in the 256x256 bg
			// (map_x , map_y ) -> map coordinates of the current tile
			// (tile_x, tile_y) -> position of the pixel in the tile
			int SCX = core->memory.read(GBIO::SCX);
			int SCY = core->memory.read(GBIO::SCY);
			int vy = (LY + SCY) % 256;
			int map_y = vy / 8;
			int tile_y = vy % 8;
			for (int x=0; x<160; x++)
			{
				int vx = (x+SCX) % 256;
				int map_x = vx/8;
				int tile_x = 7-(vx%8);
				u8 current_tile_index = VRAM[tile_map_addr+ 32*map_y + map_x] + tile_data_base;
				u16 current_tile_addr = tile_data_addr + 16*current_tile_index;
				u8 current_row_low = VRAM[current_tile_addr+2*tile_y];
				u8 current_row_high = VRAM[current_tile_addr+2*tile_y+1];
				u8 color = ((current_row_high >> tile_x)&1) << 1 | 
							((current_row_low >> tile_x)&1);

				pixels[LY*pixels_per_line+x] = colors[pallette[color]];
			}
		}
		else
		{
			for (int x=0; x<160; x++)
				pixels[LY*pixels_per_line+x] = colors[0];
		}

	}
}


int GBVideo::poll_event(SDL_Event *ev)
{
	return SDL_PollEvent(ev);
}

