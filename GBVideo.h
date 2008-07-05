#ifndef GBVIDEO_H
#define GBVIDEO_H

#include "GBMemory.h"
#include "SDL.h"

class GameBoy;

class GBVideo
{
	SDL_Surface *display;
	GameBoy *core;

	u8 VRAM[8192];
	u8 OAM[160];

	u32 colors[4];
	u32 frames_rendered;
	u32 t0;
	u8 *oldscreen, *newscreen;

	u8  cur_window_line;
	int mode;

	public:
	enum DisplayMode {
		NORMAL = 0,
		BG_MAP,
		WINDOW_MAP,
	};

	private:
	DisplayMode display_mode;
	
	public:
	static const u16 VRAM_BASE  = 0x8000;
	static const u16 OAM_BASE   = 0xFE00;

	GBVideo(GameBoy *core);
	~GBVideo();

	// VRAM/OAM access
	inline u8   read_VRAM (int addr) const { return VRAM[addr-VRAM_BASE]; }
	inline u8   read_OAM  (int addr) const { return OAM[addr-OAM_BASE]; } 
	inline u16  read16_VRAM (int addr) const { return VRAM[addr-VRAM_BASE]+(VRAM[addr-VRAM_BASE+1] << 8); }
	inline u16  read16_OAM  (int addr) const { return OAM[addr-OAM_BASE]+(OAM[addr-OAM_BASE+1] << 8); }
	inline void write_VRAM(int addr, u8 value) { VRAM[addr-VRAM_BASE] = value; }
	inline void write_OAM (int addr, u8 value) { OAM[addr-OAM_BASE] = value; }

	// drawing control
	void draw();
	u32 update();
	void set_display_mode(DisplayMode mode) { display_mode = mode; }

	// event processing
	int poll_event(SDL_Event *ev);
	
	// status queries
	u32 get_frames_rendered() { return frames_rendered; }
	
	// prevent object copying
	private:
	GBVideo(const GBVideo&);
	GBVideo operator=(const GBVideo&);

};

#endif

