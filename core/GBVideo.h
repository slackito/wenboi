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
#ifndef GBVIDEO_H
#define GBVIDEO_H

#include "GBMemory.h"
#include "util.h"

class GameBoy;

class GBVideo
{
	struct Sprite
	{
		u8 y; // plus 16
		u8 x; // plus 8
		u8 tile;
		u8 flags;

		static const u8 NON_CGB_PAL_NUMBER = 0x10;
		static const u8 X_FLIP = 0x20;
		static const u8 Y_FLIP = 0x40;
		static const u8	OBJ_BG_PRIORITY=0x80;

		bool operator< (const Sprite& other) const { return (x < other.x); }
	};

	u8 VRAM[8192];
	union {
		u8 raw[160];
		Sprite sprites[40];
	} OAM;

	enum OBJSize {
		EIGHT_BY_EIGHT=0,
		EIGHT_BY_SIXTEEN=1,
	};
	
	
	GameBoy *core;
	
	u8  cur_window_line;
	int mode;
	bool OAM_BUSY;
	bool VRAM_BUSY;

	u32 colors[4];
	u32 frames_rendered;
	u8 *screen, *oldscreen, *newscreen;

	public:
	enum DisplayMode {
		NORMAL = 0,
		BG_MAP,
		WINDOW_MAP,
		SPRITE_MAP,
	};

	private:
	DisplayMode display_mode;
    bool background_enabled;
    bool window_enabled;
    bool sprites_enabled;
	
	public:
	int cycles_until_next_update;
	static const u16 VRAM_BASE  = 0x8000;
	static const u16 OAM_BASE   = 0xFE00;

	GBVideo(GameBoy *core);
	~GBVideo();

	void reset();

	const u8 *get_screen_buffer() { return screen; }

	// VRAM/OAM access
	u8   read_VRAM (int addr) const;
	u8   read_OAM  (int addr) const;
	u16  read16_VRAM (int addr) const;
	u16  read16_OAM  (int addr) const;
	void write_VRAM(int addr, u8 value);
	void write_OAM (int addr, u8 value);

	// Write the whole OAM area via DMA
	void DMA_OAM (const u16 src);

	// drawing control
    void draw_background(bool b) { background_enabled = b; }
    void draw_window(bool b)     { window_enabled = b; }
    void draw_sprites(bool b)    { sprites_enabled = b; }
	void draw();
	u32 update();
	void set_display_mode(DisplayMode mode) { display_mode = mode; }

	// status queries
	u32 get_frames_rendered() const { return frames_rendered; }
	
	// prevent object copying
	private:
	GBVideo(const GBVideo&);
	GBVideo operator=(const GBVideo&);

};

#endif

