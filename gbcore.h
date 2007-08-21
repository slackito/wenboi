#ifndef GBCORE_H
#define GBCORE_H

#include "sized_types.h"
#include "GBMemory.h"
#include <string>

union GBRom;

class GameBoy
{
	enum GameBoyType { GAMEBOY, GAMEBOYCOLOR, SUPERGAMEBOY } gameboy_type;

	friend class GBMemory;
	GBMemory memory;
	GBRom *rom;

	enum flags_enum
	{
		ZERO_FLAG=0x80,
		ADD_SUB_FLAG=0x40,
		HALF_CARRY_FLAG=0x20,
		CARRY_FLAG=0x10,
	};

	// CPU Registers
	// ENDIANNESS WARNING!
	struct 
	{
		union 
		{
			u16 AF;
			struct { u8 flags; u8 A; };
		};
		union 
		{
			u16 BC;
			struct { u8 C; u8 B; };
		};
		union 
		{
			u16 DE;
			struct { u8 E; u8 D; };
		};
		union 
		{
			u16 HL;
			struct { u8 L; u8 H; };
		};
		u16 SP;
		u16 PC;

	} __attribute__((packed)) regs;


	u8 IME; // Interrupt master enable flag
	u8 HALT; // Is the CPU halted waiting for an interrupt?
	u32 cycle_count;

	void set_flag(const u8 f) { regs.flags |= f; }
	void reset_flag(const u8 f) { regs.flags &= (~f); }
	bool check_flag(const u8 f) { return ((regs.flags & f) != 0); }

	public:
	GameBoy(std::string rom_name, GameBoyType type=GAMEBOY);

	void reset();
	void run_cycle();
	void run();

};

#endif
