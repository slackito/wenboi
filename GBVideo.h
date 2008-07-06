#ifndef GBVIDEO_H
#define GBVIDEO_H

#include "GBMemory.h"
#include "SDL.h"

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

	SDL_Surface *display;
	GameBoy *core;

	u8 VRAM[8192];
	union {
		u8 raw[160];
		Sprite sprites[40];
	} OAM;

	enum OBJSize {
		EIGHT_BY_EIGHT=0,
		EIGHT_BY_SIXTEEN=1,
	};

	u32 colors[4];
	u32 frames_rendered;
	u32 t0;
	u32 t_last_frame;
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
	inline u8   read_OAM  (int addr) const { return OAM.raw[addr-OAM_BASE]; } 
	inline u16  read16_VRAM (int addr) const { return VRAM[addr-VRAM_BASE]+(VRAM[addr-VRAM_BASE+1] << 8); }
	inline u16  read16_OAM  (int addr) const { return OAM.raw[addr-OAM_BASE]+(OAM.raw[addr-OAM_BASE+1] << 8); }
	inline void write_VRAM(int addr, u8 value) { VRAM[addr-VRAM_BASE] = value; }
	inline void write_OAM (int addr, u8 value) { OAM.raw[addr-OAM_BASE] = value; }

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

