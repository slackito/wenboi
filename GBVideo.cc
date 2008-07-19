#include "GBVideo.h"
#include "gbcore.h"
#include "Logger.h"
#include "util.h"
#include <iostream>
#include <algorithm>
#include <vector>


GBVideo::GBVideo(GameBoy *core):
	OAM(),
	display(0),
	core(core),
	cur_window_line(0),
	mode(2),
	OAM_BUSY(false),
	VRAM_BUSY(false),
	frames_rendered(0),
	t0(0),
	t_last_frame(0),
	oldscreen(0), newscreen(0),
	display_mode(NORMAL),
	cycles_until_next_update(0)
{
	SDL_Init(SDL_INIT_VIDEO);
	display=SDL_SetVideoMode(320,288,32,SDL_SWSURFACE | SDL_DOUBLEBUF);

	t0 = SDL_GetTicks();
	oldscreen = new u8[160*144];
	newscreen = new u8[160*144];

	colors[0] = SDL_MapRGB(display->format, 192,192,0);
	colors[1] = SDL_MapRGB(display->format, 139,139,21);
	colors[2] = SDL_MapRGB(display->format, 101,101,42);
	colors[3] = SDL_MapRGB(display->format, 64,64,64);
	// colors[0] = SDL_MapRGB(display->format, 0xFF, 0xFF, 0xFF);
	// colors[1] = SDL_MapRGB(display->format, 0xAA, 0xAA, 0xAA);
	// colors[2] = SDL_MapRGB(display->format, 0x55, 0x55, 0x55);
	// colors[3] = SDL_MapRGB(display->format, 0x00, 0x00, 0x00);
}

GBVideo::~GBVideo()
{
	SDL_Quit();
	delete [] oldscreen;
	delete [] newscreen;
}

void GBVideo::reset()
{
	cycles_until_next_update = 0;
}

void GBVideo::DMA_OAM (const u16 src)
{
	for (u16 i=0; i<160; i++)
	{
		OAM.raw[i] = core->memory.read(src+i);
	}
}

u8   GBVideo::read_VRAM (int addr) const
{ 
	if (!VRAM_BUSY || 
		!check_bit(core->memory.high[GBMemory::I_LCDC],7) ) 
		return VRAM[addr-VRAM_BASE]; 
	else return 0xFF;
}

u8   GBVideo::read_OAM  (int addr) const 
{
	if (!OAM_BUSY ||
		!check_bit(core->memory.high[GBMemory::I_LCDC],7) ) 
		return OAM.raw[addr-OAM_BASE]; 
	else return 0xFF;			
} 

u16  GBVideo::read16_VRAM (int addr) const 
{ 
	if (!VRAM_BUSY ||
		!check_bit(core->memory.high[GBMemory::I_LCDC],7) ) 
		return VRAM[addr-VRAM_BASE]+(VRAM[addr-VRAM_BASE+1] << 8);
	else return 0xFF;
}

u16  GBVideo::read16_OAM  (int addr) const 
{
	if (!OAM_BUSY ||
		!check_bit(core->memory.high[GBMemory::I_LCDC],7) ) 
			return OAM.raw[addr-OAM_BASE]+(OAM.raw[addr-OAM_BASE+1] << 8); 
	else return 0xFF;
}

void GBVideo::write_VRAM(int addr, u8 value) 
{
	if (!VRAM_BUSY ||
		!check_bit(core->memory.high[GBMemory::I_LCDC],7) ) 
		VRAM[addr-VRAM_BASE] = value; 
}

void GBVideo::write_OAM (int addr, u8 value) 
{ 
	if (!OAM_BUSY ||
		!check_bit(core->memory.high[GBMemory::I_LCDC],7) ) 
		OAM.raw[addr-OAM_BASE] = value;
}

u32 GBVideo::update()
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

	int STAT = core->memory.high[GBMemory::I_STAT];
	int LYC  = core->memory.high[GBMemory::I_LYC];
	int LY  = core->memory.high[GBMemory::I_LY];

	switch (mode)
	{
		case 0:
			// HBlank (preserve bits 2-6, mode = 0)
			STAT = (STAT&0xFC);
			//OAM_BUSY  = false;
			//VRAM_BUSY = false;
			if (check_bit(STAT, 3))
			{
				logger.trace("Requesting IRQ_LCD_STAT -- HBLANK");
				core->irq(GameBoy::IRQ_LCD_STAT);
			}
			cycles_until_next_update = 204;
			if (LY == 143)
				mode = 1;
			else
				mode = 2;
			break;
		case 1:
			//OAM_BUSY  = false;
			//VRAM_BUSY = false;
			if (LY == 144)
			{
				logger.trace("Requesting IRQ_VBLANK");
				core->irq(GameBoy::IRQ_VBLANK);

				if (check_bit(STAT,4))
				{
					logger.trace("Requesting IRQ_LCD_STAT -- VBLANK");
					core->irq(GameBoy::IRQ_LCD_STAT);
				}

				if (display_mode == NORMAL)
				{
					
					u32 *pixels = static_cast<u32*>(display->pixels);
					u32 pixels_per_line = display->pitch/display->format->BytesPerPixel;
					for (int y=0; y<144; y++)
						for (int x=0; x<160; x++)
						{
							u32 offset = 160*y+x;
							u32 dst_offset = 2*y*pixels_per_line+2*x;
							u32 avg = colors[(newscreen[offset] + oldscreen[offset])>>1];
							pixels[dst_offset]                   = avg;
							pixels[dst_offset+1]                 = avg;
							pixels[dst_offset+pixels_per_line]   = avg;
							pixels[dst_offset+pixels_per_line+1] = avg;
						}
				}

				memcpy(oldscreen, newscreen, 160*144);

				SDL_Flip(display);
				frames_rendered++;
				u32 t1 = SDL_GetTicks();
				if (t1-t0 > 1000)
				{
					char buf[50];
					sprintf(buf, "%f FPS", frames_rendered/(0.001f*(t1-t0)));
					SDL_WM_SetCaption(buf, 0);
					t0=t1;
					frames_rendered=0;
				}
			
				//u32 delay = t1 - t_last_frame < 6 ? 16-(t1-t_last_frame) : 0;
				//SDL_Delay(16-delay);
				//t_last_frame = t1;

				// reset window Y
				cur_window_line=0;

				// preserve bits 3-6, set mode to 1 (VBlank) and coincidence to 0
				STAT = (STAT&0xF8) | 1;
			}
			cycles_until_next_update = 456;
			if (LY == 153)
				mode = 2;
			else
				mode = 1;
			break;
		case 2: {
			//OAM_BUSY  = true;
			//VRAM_BUSY = false;
			if (LY == LYC)
			{
				STAT = set_bit(STAT, 2); // set coincidence flag
				if (check_bit(STAT, 6))
				{
					logger.trace("Requesting IRQ_LCD_STAT -- LY = LYC = ", LY, " EI=", int(core->memory.IE), 
							" IME =", int(core->IME));
					core->irq(GameBoy::IRQ_LCD_STAT);
				}
			}

			if (check_bit(STAT, 5)) 
			{
				logger.trace("Requesting IRQ_LCD_STAT -- Mode 2");
				core->irq(GameBoy::IRQ_LCD_STAT);
			}

			// preserve bits 2-6, set mode 2
			STAT = (STAT&0xFC) | 2;
			cycles_until_next_update = 80;
			mode = 3;
			break;
		}
		case 3:
			//OAM_BUSY  = false;
			//VRAM_BUSY = true;
			draw();
			// preserve bits 2-6, set mode 3
			STAT = (STAT&0xFC) | 3;
			cycles_until_next_update = 172;
			mode = 0;
			break;
	}
	
	if (mode == 1 || mode == 2)
	{
		LY = (LY+1)%154;
		//logger.trace(LY);
		core->memory.high[GBMemory::I_LY] = LY;
	}


	core->memory.high[GBMemory::I_STAT] = STAT;

	return cycles_until_next_update;
}

void GBVideo::draw()
{

	int LCDC = core->memory.high[GBMemory::I_LCDC];
	int LY  = core->memory.high[GBMemory::I_LY];
	int WY  = core->memory.high[GBMemory::I_WY];

	if (LY < 144 && display_mode == NORMAL)
	{
		int BGP  = core->memory.high[GBMemory::I_BGP];
		int bg_pal[4];
		bg_pal[0] = BGP & 3;
		bg_pal[1] = (BGP>>2) & 3;
		bg_pal[2] = (BGP>>4) & 3;
		bg_pal[3] = (BGP>>6) & 3;
		
		int OBP0 = core->memory.high[GBMemory::I_OBP0];
		int ob_pal0[4];
		ob_pal0[0] = OBP0 & 3;
		ob_pal0[1] = (OBP0>>2) & 3;
		ob_pal0[2] = (OBP0>>4) & 3;
		ob_pal0[3] = (OBP0>>6) & 3;
		int OBP1 = core->memory.high[GBMemory::I_OBP1];
		int ob_pal1[4];
		ob_pal1[0] = OBP1 & 3;
		ob_pal1[1] = (OBP1>>2) & 3;
		ob_pal1[2] = (OBP1>>4) & 3;
		ob_pal1[3] = (OBP1>>6) & 3;
			
		int line_base = 160*LY;

		// Draw the background
		if (check_bit(LCDC, 0))  // is BG display active?
		{
			u16 tile_map_addr  = check_bit(LCDC,3) ? 0x1C00  : 0x1800;
			u16 tile_data_addr = check_bit(LCDC,4) ? 0x0000 : 0x0800;
			int tile_data_base = (tile_data_addr == 0x0800) ? -128 : 0;

			// (vx    , vy    ) -> position of the pixel in the 256x256 bg
			// (map_x , map_y ) -> map coordinates of the current tile
			// (tile_x, tile_y) -> position of the pixel in the tile
			int SCX = core->memory.high[GBMemory::I_SCX];
			int SCY = core->memory.high[GBMemory::I_SCY];
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

				newscreen[line_base+x] = color;
			}
		}
		else
		{
			for (int x=0; x<160; x++)
			{
				newscreen[line_base+x] = 0;
			}
		}
	
		//logger.trace("LCDC=0x", std::hex, LCDC, " LY=", LY, " WY=", WY);
		// Draw the window
		if (check_bit(LCDC, 5) && LY > WY && LY < 144)  // is BG display active?
		{
			u16 tile_map_addr  = check_bit(LCDC,6) ? 0x1C00  : 0x1800;
			u16 tile_data_addr = check_bit(LCDC,4) ? 0x0000 : 0x0800;
			int tile_data_base = (tile_data_addr == 0x0800) ? -128 : 0;

			// (vx    , vy    ) -> position of the pixel in the 256x256 bg
			// (map_x , map_y ) -> map coordinates of the current tile
			// (tile_x, tile_y) -> position of the pixel in the tile
			int WX = core->memory.high[GBMemory::I_WX] - 7;

			logger.trace("Drawing window at (", WX, ",", WY, ")\tLY=", LY, " cur_window_line=", cur_window_line);

			if (WX < 160)
			{
				u8  min_x = WX < 0 ? 0 : WX;

				int vy = cur_window_line++;
				int map_y = vy / 8;
				int tile_y = vy % 8;
				for (int x=min_x; x<160; x++)
				{
					int vx = x-WX;
					int map_x = vx/8;
					
					int tile_x = 7-(vx%8);
					u8 current_tile_index = VRAM[tile_map_addr+ 32*map_y + map_x] + tile_data_base;
					u16 current_tile_addr = tile_data_addr + 16*current_tile_index;
					u8 current_row_low = VRAM[current_tile_addr+2*tile_y];
					u8 current_row_high = VRAM[current_tile_addr+2*tile_y+1];
					u8 color = ((current_row_high >> tile_x)&1) << 1 | 
								((current_row_low >> tile_x)&1);

					newscreen[line_base+x] = color;
				}
			}
		}

		// ------- SPRITES
		if (check_bit(LCDC, 1))
		{
			int sprite_size = (LCDC & 0x4) >> 2;
			std::vector<Sprite> v;
			int sprite_height = sprite_size == EIGHT_BY_EIGHT? 8 : 16;

			// find sprites in current line
			for (int i=0; i<40; i++)
			{
				int y_orig = OAM.sprites[i].y - 16;
				if (LY >= y_orig && LY < y_orig+sprite_height)
					v.push_back(OAM.sprites[i]);
			}

			// sort sprites
			std::stable_sort(v.begin(), v.end());

			// draw sprites
			u16 tile_data_addr = 0x0000;
			int cur_x = 0;
			for (u32 i=0; i<v.size() && i<10 ; i++)
			{
				int screen_x = std::max(cur_x, v[i].x-8);
				int sprite_x = screen_x - (v[i].x-8);
				int sprite_y = LY - v[i].y;
				int current_tile_index = v[i].tile;
				if (sprite_height == 16)
				{
					if (sprite_y < 8)
					{
						current_tile_index &= 0xFE;
					}
					else
					{
						current_tile_index |= 1;
						sprite_y -= 8;
					}
				}
				int pal_num = (v[i].flags & Sprite::NON_CGB_PAL_NUMBER) >> 4;
				for (int x=sprite_x; x < 8; x++)
				{
					u16 current_tile_addr = tile_data_addr + 16*current_tile_index;
					u8 current_row_low = VRAM[current_tile_addr+2*sprite_y];
					u8 current_row_high = VRAM[current_tile_addr+2*sprite_y+1];
					u8 color = ((current_row_high >> sprite_x)&1) << 1 | 
								((current_row_low >> sprite_x)&1);

					newscreen[line_base+screen_x+x] = color | (pal_num + 1 << 4);
					logger.trace(pal_num + 1 << 4);
				}

				cur_x = v[i].x;
			}
		}	
		// Apply pallettes to the line
		for (int x=0; x<160; x++)
		{
			u8 pixel = newscreen[line_base+x];
			u8 pal_tag = pixel >> 4;
			u8 color = pixel & 0x3;
			if (pal_tag == 0)
				newscreen[line_base+x] = bg_pal[color];
			else if (pal_tag == 1)
				newscreen[line_base+x] = ob_pal0[color];
			else
				newscreen[line_base+x] = ob_pal1[color];

		}

	}
	else if (display_mode == BG_MAP)
	{
		if (LY==0)
		{
			u32 *pixels = static_cast<u32*>(display->pixels);
			int BGP  = core->memory.high[GBMemory::I_BGP];
			int pallette[4];
			pallette[0] = BGP & 3;
			pallette[1] = (BGP>>2) & 3;
			pallette[2] = (BGP>>4) & 3;
			pallette[3] = (BGP>>6) & 3;
			u16 tile_map_addr  = check_bit(LCDC,3) ? 0x1C00  : 0x1800;
			u16 tile_data_addr = check_bit(LCDC,4) ? 0x0000 : 0x0800;
			int tile_data_base = (tile_data_addr == 0x0800) ? -128 : 0;
			for (int row=0; row < 32; row++)
			{
				logger.trace("bgmap row=", row);
				for (int col=0; col < 32; col++)
				{
					int ty = row*8;
					int tx = col*8;
					for (int y=0; y<8; y++)
					{
						for (int x=0; x<8; x++)
						{
							u8 tile_x = 7-x;
							u8 current_tile_index = VRAM[tile_map_addr+32*row + col] + tile_data_base;
							u16 current_tile_addr = tile_data_addr + 16*current_tile_index;
							u8 current_row_low = VRAM[current_tile_addr+2*y];
							u8 current_row_high = VRAM[current_tile_addr+2*y+1];
							u32 color = colors[pallette[((current_row_high >> tile_x)&1) << 1 | 
										((current_row_low >> tile_x)&1)]];
							pixels[320*(ty+y)+(tx+x)] = color;
						}
					}
				}
			}
		}
	}
	else if (display_mode == WINDOW_MAP)
	{
		if (LY==0)
		{
			u32 *pixels = static_cast<u32*>(display->pixels);
			int BGP  = core->memory.high[GBMemory::I_BGP];
			int pallette[4];
			pallette[0] = BGP & 3;
			pallette[1] = (BGP>>2) & 3;
			pallette[2] = (BGP>>4) & 3;
			pallette[3] = (BGP>>6) & 3;
			u16 tile_map_addr  = check_bit(LCDC,6) ? 0x1C00  : 0x1800;
			u16 tile_data_addr = check_bit(LCDC,4) ? 0x0000 : 0x0800;
			int tile_data_base = (tile_data_addr == 0x0800) ? -128 : 0;
			for (int row=0; row < 32; row++)
			{
				logger.trace("bgmap row=", row);
				for (int col=0; col < 32; col++)
				{
					int ty = row*8;
					int tx = col*8;
					for (int y=0; y<8; y++)
					{
						for (int x=0; x<8; x++)
						{
							u8 tile_x = 7-x;
							u8 current_tile_index = VRAM[tile_map_addr+32*row + col] + tile_data_base;
							u16 current_tile_addr = tile_data_addr + 16*current_tile_index;
							u8 current_row_low = VRAM[current_tile_addr+2*y];
							u8 current_row_high = VRAM[current_tile_addr+2*y+1];
							u32 color = colors[pallette[((current_row_high >> tile_x)&1) << 1 |
										((current_row_low >> tile_x)&1)]];
							pixels[320*(ty+y)+(tx+x)] = color;
						}
					}
				}
			}
		}
	}
	else if (display_mode == SPRITE_MAP)
	{
		if (LY==0)
		{
			u32 *pixels = static_cast<u32*>(display->pixels);
			int BGP  = core->memory.high[GBMemory::I_BGP];
			int pallette[4];
			pallette[0] = BGP & 3;
			pallette[1] = (BGP>>2) & 3;
			pallette[2] = (BGP>>4) & 3;
			pallette[3] = (BGP>>6) & 3;
			u16 tile_map_addr  = check_bit(LCDC,6) ? 0x1C00  : 0x1800;
			u16 tile_data_addr = 0x0000;
			int tile_data_base = (tile_data_addr == 0x0800) ? -128 : 0;
			for (int row=0; row < 32; row++)
			{
				logger.trace("bgmap row=", row);
				for (int col=0; col < 32; col++)
				{
					int ty = row*8;
					int tx = col*8;
					for (int y=0; y<8; y++)
					{
						for (int x=0; x<8; x++)
						{
							u8 tile_x = 7-x;
							u8 current_tile_index = VRAM[tile_map_addr+32*row + col] + tile_data_base;
							u16 current_tile_addr = tile_data_addr + 16*current_tile_index;
							u8 current_row_low = VRAM[current_tile_addr+2*y];
							u8 current_row_high = VRAM[current_tile_addr+2*y+1];
							u32 color = colors[pallette[((current_row_high >> tile_x)&1) << 1 |
										((current_row_low >> tile_x)&1)]];
							pixels[320*(ty+y)+(tx+x)] = color;
						}
					}
				}
			}
		}
	}
}

int GBVideo::poll_event(SDL_Event *ev)
{
	return SDL_PollEvent(ev);
}

