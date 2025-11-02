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
#ifndef GBCORE_H
#define GBCORE_H

#include <iomanip>
#include <map>
#include <string>

#include "../common/Logger.h"
#include "../common/sized_types.h"
#include "GBMemory.h"
#include "GBVideo.h"
#include "Instruction.h"

union GBRom;

/* A GameBoy with debugging facilities :) */
class GameBoy {
public:
  enum GameBoyType { GAMEBOY, GAMEBOYCOLOR, SUPERGAMEBOY } gameboy_type;
  enum Control {
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_START,
    BUTTON_SELECT,
    PAD_UP,
    PAD_DOWN,
    PAD_LEFT,
    PAD_RIGHT,
    NUM_CONTROLS
  };

  enum run_status {
    NORMAL = 0,
    BREAKPOINT,
    WATCHPOINT,
    TRACEPOINT,
    STEP,
    PAUSED,
    QUIT,
    WAIT,
  };

  enum InterruptRequest {
    IRQ_VBLANK = 0x01,
    IRQ_LCD_STAT = 0x02,
    IRQ_TIMER = 0x04,
    IRQ_SERIAL = 0x08,
    IRQ_JOYPAD = 0x10,
  };

  struct Breakpoint {
    int addr;
    bool enabled;

    Breakpoint(int a, bool e) : addr(a), enabled(e) {}
    Breakpoint() : addr(-1), enabled(false) {}
  };

  typedef std::map<int, Breakpoint> BreakpointMap;

  // Constructors / destructors
  GameBoy(GameBoyType type = GAMEBOY);
  ~GameBoy();

  void load_ROM(std::string rom_name);

  // running control methods
  void irq(InterruptRequest i) {
    memory.write(0xFF0F, memory.read(0xFF0F) | i);
  }
  void reset();
  run_status run_cycle();
  run_status run();

  // button control methods
  void push_control(Control b);
  void release_control(Control b);

  // video output
  const u8 *get_screen_buffer() { return video.get_screen_buffer(); }

  // debug methods
  int set_breakpoint(u16 addr);
  void delete_breakpoint(int id);
  void enable_breakpoint(int id);
  void disable_breakpoint(int id);
  const BreakpointMap get_breakpoints() const { return breakpoints; }

  std::string status_string();
  Instruction disassemble_opcode(u16 addr);
  static std::string get_port_name(int port);

  friend class GBMemory;
  friend class GBVideo;
  GBMemory memory;
  GBVideo video;
  GBRom *rom;

  bool controls[NUM_CONTROLS];
  static bool is_pad[NUM_CONTROLS];
  enum Flag {
    ZERO_FLAG = 0x80,
    ADD_SUB_FLAG = 0x40,
    HALF_CARRY_FLAG = 0x20,
    CARRY_FLAG = 0x10,
  };

  // CPU Registers
  // ENDIANNESS WARNING!
  struct RegisterSet {
    union {
      u16 AF;
      struct {
        u8 flags;
        u8 A;
      };
    };
    union {
      u16 BC;
      struct {
        u8 C;
        u8 B;
      };
    };
    union {
      u16 DE;
      struct {
        u8 E;
        u8 D;
      };
    };
    union {
      u16 HL;
      struct {
        u8 L;
        u8 H;
      };
    };
    u16 SP;
    u16 PC;

  } regs;

  void set_flag(Flag f) { regs.flags |= f; }
  void reset_flag(Flag f) { regs.flags &= (~f); }
  int check_flag(Flag f) const { return ((regs.flags & f) != 0 ? 1 : 0); }

  u8 IME;  // Interrupt master enable flag
  u8 HALT; // Is the CPU halted waiting for an interrupt?
  u8 STOP; // Is the CPU & LCD halted waiting for a keypress?

  u32 cycle_count;
  u32 cycles_until_next_instruction;
  u8 divider_count; // resets every 256 cycles, so we don't need a cmp
  u32 timer_count;
  static const u32 CYCLE_STEP = 4;

private:
  inline void do_call(u16 addr) {
    logger.debug("do_call(0x", std::hex, std::setw(4), std::setfill('0'), addr,
                 ")");
    memory.write(regs.SP - 1, regs.PC >> 8);
    memory.write(regs.SP - 2, regs.PC & 0xFF);
    regs.SP -= 2;
    regs.PC = addr;
  }

  // prevent object copying
  GameBoy(const GameBoy &);
  GameBoy operator=(const GameBoy &);

  // update JOYP register when controls are pushed/released
  void update_JOYP();

  // free ROM (used in destructor and load_rom)
  void free_ROM();

  BreakpointMap breakpoints;
  int last_breakpoint_id;
};

#endif
