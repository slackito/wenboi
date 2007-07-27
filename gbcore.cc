#include "gbcore.h"

#include "sized_types.h"
#include "GBRom.h"
#include "GBMemory.h"
#include "MBC.h"
#include "logger.h"
#include <string>
#include <cstring>

class MBC
{
	virtual u8 read_byte() const=0;
	virtual void write_byte(u8)=0;
};

class GBMemory
{
	MBC *mbc;
	                // 0000-3FFF: ROM Bank 0 (in cart)
			// 4000-7FFF: Switchable ROM Bank (in cart)
	u8 VRAM[8192];  // 8000-9FFF: Video RAM
			// A000-BFFF: External RAM (in cart, switchable)
	u8 WRAM0[4096]; // C000-CFFF: Work RAM Bank 0
	u8 WRAM1[4096]; // D000-DFFF: Work RAM Bank 1 (TODO: In GBC mode switchable bank 1-7)
			// E000-FDFF: ECHO: Same as C000-DDFF
	u8 OAM[160];    // FE00-FE9F: Sprite Attribute Table
	u8 HRAM[126];   // FF80-FFFE: High RAM

	public:
	GBMemory(): mbc(0) {}
	void init(MBC *mbc) { this->mbc = mbc; }


	int& operator[](int index)=0;
	int  operator[](int index) const=0;

};

class GameBoy
{
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

	void set_flag(const u8 f) { regs.flags |= f; }
	void reset_flag(const u8 f) { regs.flags &= (~f); }
	bool check_flag(const u8 f) { return (regs.flags & f != 0); }

	public:
	GameBoy(std::string rom_name);

	void reset();
	void run_cycle();
	void run();

};

GameBoy::GameBoy(std::string rom_name):
	rom(0), regs(), IME(1), HALT(0)
{
	logger.info("GameBoy init");
	rom = read_gbrom(rom_name);
	reset();
}

void GameBoy::reset()
{
	logger.info("GameBoy reset");
	std::memcpy(memory, rom->data, 16384);
	regs.PC = 0x100;
}

#include "opcodes.h"

void GameBoy::run_cycle()
{
	int prefix;
	int opcode;
	opcode = memory[regs.PC++];
	if (opcode == 0xCB)
	{
		prefix=opcode;
		opcode=memory[regs.PC++];
	}

	switch(opcode)
	{
		// LD n, nn
		for_each_register(0x3E, 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, LD_reg_nn)

		// LD r1,r2
		for_each_register(0x7F, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, LD_A_reg)
		for_each_register(0x47, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, LD_B_reg)
		for_each_register(0x4F, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, LD_C_reg)
		for_each_register(0x57, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, LD_D_reg)
		for_each_register(0x5F, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, LD_E_reg)
		for_each_register(0x67, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, LD_H_reg)
		for_each_register(0x6F, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, LD_L_reg)
		
		// LD reg, (HL)
		for_each_register(0x7E, 0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E, LD_reg__HL_)

		// LD (HL), reg
		for_each_register(0x77, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, LD__HL__reg)
		
		case 0x36: // LD (HL), n
			memory[regs.HL] = memory[regs.PC++];
			break;
		
		// LD A, mem
		case 0x0A: // LD A, (BC)
			regs.A = memory[regs.BC];
			break;
		case 0x1A: // LD A, (DE)
			regs.A = memory[regs.DE];
			break;
		case 0xFA: // LD A, (nn)
			regs.A = memory[memory[regs.PC] + memory[regs.PC+1]<<8];
			regs.PC+=2;
			break;
		
		// LD mem, A
		case 0x02: // LD (BC), A
			memory[regs.BC] = regs.A;
			break;
		case 0x12: // LD (DE), A
			memory[regs.DE] = regs.A;
			break;
		case 0xEA: // LD (nn), A
			memory[memory[regs.PC] + memory[regs.PC+1]<<8] = regs.A;
			break;

		// LD A, (C)
		case 0xF2:
			regs.A = memory[0xFF00 + regs.C];
			break;
		// LD (C), A
		case 0xE2:
			memory[0xFF00 + regs.C] = regs.A;
			break;

		// LD A, (HLD); LD A, (HL-); LDD A,(HL);
		case 0x3A:
			regs.A = memory[regs.HL];
			--regs.HL;
			break;
		// LD (HLD), A; LD (HL-), A; LDD (HL), A;
		case 0x32:
			memory[regs.HL] = regs.A;
			--regs.HL;
			break;
		// LD A, (HLI); LD A, (HL+); LDI A, (HL);
		case 0x2A:
			regs.A = memory[regs.HL];
			++regs.HL;
			break;
		// LD (HLI), A; LD (HL+), A; LDI (HL), A;
		case 0x22:
			memory[regs.HL] = regs.A;
			++regs.HL;
			break;

		// LDH (n), A
		case 0xE0:
			memory[0xFF00 + regs.PC++] = regs.A;
			break;
		// LDH A, (n)
		case 0xF0:
			regs.A = memory[0xFF00 + regs.PC++];
			break;

		// LD n, nn
		case 0x01: // LD BC, nn
			regs.BC = memory[regs.PC]+(memory[regs.PC+1] << 8);
			regs.PC +=2;
			break;
		case 0x11: // LD DE, nn
			regs.DE = memory[regs.PC]+(memory[regs.PC+1] << 8);
			regs.PC +=2;
			break;
		case 0x21: // LD HL, nn
			regs.HL = memory[regs.PC]+(memory[regs.PC+1] << 8);
			regs.PC +=2;
			break;
		case 0x31: // LD SP, nn
			regs.SP = memory[regs.PC]+(memory[regs.PC+1] << 8);
			regs.PC +=2;
			break;
		
		// LD SP, HL
		case 0xF9:
			regs.SP = regs.HL;
			break;

		// LD HL, SP+n
		// LDHL SP, n
		case 0xF8: {
			s8 offset = *(reinterpret_cast<s8*>(memory+regs.PC++));
			int res = regs.SP + offset;
			
			// TODO: Verificar si los flags van asi
			set_flag_if (res > 0xFFFF, CARRY_FLAG);

			// TODO: hacer lo apropiado con el half-carry flag
			reset_flag(ADD_SUB_FLAG);
			reset_flag(ZERO_FLAG);

			regs.HL = static_cast<u16>(res & 0xFFFF);
			break; 
			}

		// LD (nn), SP
		case 0x08: {
			int addr = memory[regs.PC] + memory[regs.PC+1] << 8;
			regs.PC += 2;
			memory[addr] = regs.SP;
			break;
			}

		// PUSH nn
		PUSH(0xF5, A, flags)
		PUSH(0xC5, B, C)
		PUSH(0xD5, D, E)
		PUSH(0xE5, H, L)

		// POP nn
		POP(0xF1, A, flags)
		POP(0xC1, B, C)
		POP(0xD1, D, E)
		POP(0xE1, H, L)

		// 8-bit ALU
		// ADD A,reg
		for_each_register(0x87, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, ADD_A_reg)
		
		case 0x86: {// ADD A, (HL)
			int res = regs.A + memory[regs.HL];
			int half_res = (regs.A & 0x0F) + (memory[regs.HL] & 0x0F);
			regs.A = static_cast<u8>(res);
			
			reset_flag(ADD_SUB_FLAG);
			set_flag_if (res > 0xFF,      CARRY_FLAG);
			set_flag_if (regs.A == 0,     ZERO_FLAG);
			set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG);
			break;
			}
		case 0xC6: {//ADD A, #
			int inm = memory[regs.PC++];
			int res = regs.A + inm;
			int half_res = (regs.A & 0x0F) + (inm & 0x0F);
			regs.A = static_cast<u8>(res);
			
			reset_flag(ADD_SUB_FLAG);
			set_flag_if (res > 0xFF,      CARRY_FLAG);
			set_flag_if (regs.A == 0,     ZERO_FLAG);
			set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG);
			break;
			}

		// ADC A, n
		for_each_register(0x8F, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, ADC_A_reg)

		case 0x8E: {// ADC A, (HL)
			int carry = (check_flag(CARRY_FLAG)? 1 : 0);
			int res = regs.A + memory[regs.HL] + carry;
			int half_res = (regs.A & 0x0F) + (memory[regs.HL] & 0x0F) + carry;
			regs.A = static_cast<u8>(res);
			
			reset_flag(ADD_SUB_FLAG);
			set_flag_if (res > 0xFF,      CARRY_FLAG);
			set_flag_if (regs.A == 0,     ZERO_FLAG);
			set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG);
			break;
			}
		case 0xCE: {//ADC A, #
			int carry = (check_flag(CARRY_FLAG)? 1 : 0);
			int inm = memory[regs.PC++];
			int res = regs.A + inm + carry;
			int half_res = (regs.A & 0x0F) + (inm & 0x0F) + carry;
			regs.A = static_cast<u8>(res);
			
			reset_flag(ADD_SUB_FLAG);
			set_flag_if (res > 0xFF,      CARRY_FLAG);
			set_flag_if (regs.A == 0,     ZERO_FLAG);
			set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG);
			break;
			}

		// SUB n
		for_each_register(0x97, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, SUB_reg)

		case 0x96: {//SUB (HL)
			int res = regs.A - memory[regs.HL];
			int half_res = (regs.A & 0x0F) - (memory[regs.HL] & 0x0F);
			regs.A = static_cast<u8>(res);

			set_flag(ADD_SUB_FLAG);
			set_flag_if (res < 0,      CARRY_FLAG);
			set_flag_if (res == 0,     ZERO_FLAG);
			set_flag_if (half_res < 0, HALF_CARRY_FLAG);
			break;
			}
		
		case 0xD6: {//SUB #
			int inm = memory[regs.PC++];
			int res = regs.A - inm;
			int half_res = (regs.A & 0x0F) - (inm & 0x0F);
			regs.A = static_cast<u8>(res);

			set_flag(ADD_SUB_FLAG);
			set_flag_if (res < 0,      CARRY_FLAG);
			set_flag_if (res == 0,     ZERO_FLAG);
			set_flag_if (half_res < 0, HALF_CARRY_FLAG);
			break;
			}

		// SBC n
		for_each_register(0x9F, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, SBC_reg)

		case 0x9E: {//SBC (HL)
			int carry = (check_flag(CARRY_FLAG)? 1 : 0);
			int res = regs.A - memory[regs.HL] - carry;
			int half_res = (regs.A & 0x0F) - (memory[regs.HL] & 0x0F) - carry;
			regs.A = static_cast<u8>(res);

			set_flag(ADD_SUB_FLAG);
			set_flag_if (res < 0,      CARRY_FLAG);
			set_flag_if (res == 0,     ZERO_FLAG);
			set_flag_if (half_res < 0, HALF_CARRY_FLAG);
			break;
			}

		// There is no SBC inm

		// AND n
		for_each_register(0xA7, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, AND_reg)

		case 0xA6: //AND (HL)
			regs.A &= memory[regs.HL];
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			set_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		case 0xE6: //AND inm
			regs.A &= memory[regs.PC++];
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			set_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		// OR n
		for_each_register(0xB7, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, OR_reg)

		case 0xB6: //OR (HL)
			regs.A |= memory[regs.HL];
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		case 0xF6: //OR inm
			regs.A |= memory[regs.PC++];
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		// XOR n
		for_each_register(0xAF, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, XOR_reg)

		case 0xAE: //XOR (HL)
			regs.A ^= memory[regs.HL];
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		case 0xEE: //XOR inm
			regs.A ^= memory[regs.PC++];
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;
		
		// CP n
		for_each_register(0xBF, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, CP_reg)

		case 0xBE: {//SUB (HL)
			int res = regs.A - memory[regs.HL];
			int half_res = (regs.A & 0x0F) - (memory[regs.HL] & 0x0F);
			regs.A = static_cast<u8>(res);

			set_flag(ADD_SUB_FLAG);
			set_flag_if (res < 0,      CARRY_FLAG);
			set_flag_if (res == 0,     ZERO_FLAG);
			set_flag_if (half_res < 0, HALF_CARRY_FLAG);
			break;
			}
		
		case 0xFE: {//SUB #
			int inm = memory[regs.PC++];
			int res = regs.A - inm;
			int half_res = (regs.A & 0x0F) - (inm & 0x0F);
			regs.A = static_cast<u8>(res);

			set_flag(ADD_SUB_FLAG);
			set_flag_if (res < 0,      CARRY_FLAG);
			set_flag_if (res == 0,     ZERO_FLAG);
			set_flag_if (half_res < 0, HALF_CARRY_FLAG);
			break;
			}

		// INC n
		for_each_register(0x3C, 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, INC_reg)

		case 0x34: {//INC (HL)
			int half_res = (memory[regs.HL] & 0x0F) + 1; 
			++memory[regs.HL]; 
			reset_flag(ADD_SUB_FLAG); 
			set_flag_if (memory[regs.HL] == 0, ZERO_FLAG); 
			set_flag_if (half_res > 0x0F,      HALF_CARRY_FLAG); 
			break; 
			}	

		// DEC n
		for_each_register(0x3D, 0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, DEC_reg)

		case 0x35: {//DEC (HL)
			int half_res = (memory[regs.HL] & 0x0F) - 1; 
			--memory[regs.HL]; 
			set_flag(ADD_SUB_FLAG); 
			set_flag_if (memory[regs.HL] == 0, ZERO_FLAG); 
			set_flag_if (half_res < 0,         HALF_CARRY_FLAG);
			break; 
			}

		// 16-bit ALU
		// ADD HL, n
		for_each_register16(0x09, 0x19, 0x29, 0x39, ADD_HL_reg16)

		// ADD SP, #
		case 0xE8: {
			// FIXME: No se que hacer con el half carry, en 4 o en 11?
			int n = *(reinterpret_cast<s8*>(memory+regs.PC++));
			int res = regs.SP + n;
			regs.SP = static_cast<u8>(res);
			reset_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			set_flag_if(res > 0xFFFF, CARRY_FLAG);
			break;
		}

		// INC nn
		for_each_register16(0x03, 0x13, 0x23, 0x33, INC_reg16)

		// DEC nn
		for_each_register16(0x0B, 0x1B, 0x2B, 0x3B, DEC_reg16)

		// Miscellaneous instructions
		// SWAP n
		case 0xCB: {
			int sub_opcode = memory[regs.PC++];
			switch(sub_opcode)
			{
				for_each_register(0x37, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, SWAP_reg)

				// SWAP (HL)
				case 0x36: {
					u8 tmp = memory[regs.HL];
					tmp = ((tmp & 0x0F) << 4) | ((tmp & 0xF0)>>4);
					memory[regs.HL] = tmp;
		
					set_flag_if(tmp==0, ZERO_FLAG);
					reset_flag(CARRY_FLAG);
					reset_flag(HALF_CARRY_FLAG);
					reset_flag(ADD_SUB_FLAG);
					break;
				}
			}
			break;
		}

		// DAA http://www.worldofspectrum.org/faq/reference/z80reference.htm#DAA
		case 0x27: {
			u8 corr_factor = 0;
			if (regs.A > 0x99 || check_flag(CARRY_FLAG)) {
				corr_factor = 0x60;
				set_flag(CARRY_FLAG);
			} else {
				reset_flag(CARRY_FLAG);
			}

			if (regs.A & 0x0F > 9 || check_flag(HALF_CARRY_FLAG)) {
				corr_factor |= 0x06;
			}

			if (!check_flag(ADD_SUB_FLAG)) {
				regs.A += corr_factor;
			} else {
				regs.A -= corr_factor;
			}

			set_flag_if(regs.A==0, ZERO_FLAG);
			reset_flag(HALF_CARRY_FLAG); // GBCPUman.pdf contradicts previous reference :P
			break;
		}

		// CPL
		case 0x2F:
			regs.A = ~regs.A;
			set_flag(HALF_CARRY_FLAG);
			set_flag(ADD_SUB_FLAG);
			break;

		// CCF
		case 0x3F:
			if (check_flag(CARRY_FLAG))
				reset_flag(CARRY_FLAG);
			else
				set_flag(CARRY_FLAG);

			reset_flag(HALF_CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			break;

		// SCF
		case 0x37:
			set_flag(CARRY_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			break;

		// NOP
		case 0x00:
			break;

		// HALT
		case 0x76:
			HALT = true;
			break;

		// STOP
		case 0x10:
			int sub_opcode = memory[regs.PC++];
			if (sub_opcode == 0x00) {
				HALT = true;
			} else {
				logger.critical("Unknown sub-opcode after 0x10");
			}






			





			



	}

}

