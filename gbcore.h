#ifndef GBCORE_H
#define GBCORE_H

#include "sized_types.h"
#include "GBMemory.h"
#include "GBVideo.h"
#include <string>

union GBRom;

class GameBoy
{
	public:
	enum GameBoyType { GAMEBOY, GAMEBOYCOLOR, SUPERGAMEBOY } gameboy_type;
	enum InterruptRequest { 
		IRQ_VBLANK   = 0x00, 
		IRQ_LCD_STAT = 0x10,
		IRQ_TIMER    = 0x20, 
		IRQ_SERIAL   = 0x40,
		IRQ_JOYPAD   = 0x80
	};
	
	enum Flag
	{
		ZERO_FLAG       = 0x80,
		ADD_SUB_FLAG    = 0x40,
		HALF_CARRY_FLAG = 0x20,
		CARRY_FLAG      = 0x10,
	};


	friend class GBMemory;
	friend class GBVideo;
	GBMemory memory;
	GBVideo video;
	GBRom *rom;

	// CPU Registers
	// ENDIANNESS WARNING!
	struct RegisterSet
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
	
	inline void do_call(u16 addr)
	{
		memory.write(regs.SP-1, regs.PC >> 8); 
		memory.write(regs.SP-2, regs.PC & 0xFF); 
		regs.SP -= 2; 
		regs.PC = addr; 
	}

	void set_flag  (Flag f) { regs.flags |= f; }
	void reset_flag(Flag f) { regs.flags &= (~f); }
	bool check_flag(Flag f) const { return ((regs.flags & f) != 0); }

	enum run_status 
	{
		NORMAL = 0,
		BREAKPOINT = 1,
		WATCHPOINT = 2,
		TRACEPOINT = 3,
	};

	GameBoy(std::string rom_name, GameBoyType type=GAMEBOY);

	void irq(InterruptRequest i) { memory.write(0xFFFF, memory.read(0xFFFF) | i); }
	void reset();
	run_status run_cycle();
	run_status run();

	// debug methods
	void disassemble_opcode(u16 addr, std::string &instruction, int &length) const;
	std::string status_string() const;
	std::string get_port_name(int port) const;

};

#endif
