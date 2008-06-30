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

	
	public:
	static const u16 VRAM_BASE  = 0x8000;
	static const u16 OAM_BASE   = 0xFE00;

	GBVideo(GameBoy *core);
	~GBVideo();

	// VRAM/OAM access
	u8   read_VRAM (int addr) const;
	u8   read_OAM  (int addr) const;
	void write_VRAM(int addr, u8 value);
	void write_OAM (int addr, u8 value);

	// drawing control
	void update();

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

