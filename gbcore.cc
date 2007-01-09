#include "gbcore.h"

#include "sized_types.h"
#include "gbrom.h"
#include <string>
#include <cstring>

class GameBoy
{
	u8 memory[65536];
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

	void set_flag(const u8 f) { regs.flags |= f; }
	void reset_flag(const u8 f) { regs.flags &= (~f); }

	public:
	GameBoy(std::string rom_name);

	void reset();
	void run_cycle();
	void run();

};

GameBoy::GameBoy(std::string rom_name):
	rom(0), regs()
{
	rom = read_gbrom(rom_name);
	reset();
}

void GameBoy::reset()
{
	std::memcpy(memory, rom->data, 16384);
	regs.PC = 0x100;
}


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
		case 0x76: // LD A,n
			regs.A = memory[regs.PC++];
			break;
		case 0x06: // LD B,n
			regs.B = memory[regs.PC++];
			break;
		case 0x0E: // LD C,n
			regs.C = memory[regs.PC++];
			break;
		case 0x16: // LD D,n
			regs.D = memory[regs.PC++];
			break;
		case 0x1E: // LD E,n
			regs.E = memory[regs.PC++];
			break;
		case 0x26: // LD H,n
			regs.H = memory[regs.PC++];
			break;
		case 0x2E: // LD L,n
			regs.L = memory[regs.PC++];
			break;

		// LD r1,r2
		case 0x7F: // LD A,A
			regs.A = regs.A;
			break;
		case 0x78: // LD A,B
			regs.A = regs.B;
			break;
		case 0x79: // LD A,C
			regs.A = regs.C;
			break;
		case 0x7A: // LD A,D
			regs.A = regs.D;
			break;
		case 0x7B: // LD A,E
			regs.A = regs.E;
			break;
		case 0x7C: // LD A,H
			regs.A = regs.H;
			break;
		case 0x7D: // LD A,L
			regs.A = regs.L;
			break;
		case 0x7E: // LD A,(HL)
			regs.A = memory[regs.HL];
			break;

		case 0x47: // LD B,A
			regs.B = regs.A;
			break;
		case 0x40: // LD B,B
			regs.B = regs.B;
			break;
		case 0x41: // LD B,C
			regs.B = regs.C;
			break;
		case 0x42: // LD B,D
			regs.B = regs.D;
			break;
		case 0x43: // LD B,E
			regs.B = regs.E;
			break;
		case 0x44: // LD B,H
			regs.B = regs.H;
			break;
		case 0x45: // LD B,L
			regs.B = regs.L;
			break;
		case 0x46: // LD B,(HL)
			regs.B = memory[regs.HL];
			break;

		case 0x4F: // LD C,A
			regs.C = regs.A;
			break;
		case 0x48: // LD C,B
			regs.C = regs.B;
			break;
		case 0x49: // LD C,C
			regs.C = regs.C;
			break;
		case 0x4A: // LD C,D
			regs.C = regs.D;
			break;
		case 0x4B: // LD C,E
			regs.C = regs.E;
			break;
		case 0x4C: // LD C,H
			regs.C = regs.H;
			break;
		case 0x4D: // LD C,L
			regs.C = regs.L;
			break;
		case 0x4E: // LD C,(HL)
			regs.C = memory[regs.HL];
			break;

		case 0x57: // LD D,A
			regs.D = regs.A;
			break;
		case 0x50: // LD D,B
			regs.D = regs.B;
			break;
		case 0x51: // LD D,C
			regs.D = regs.C;
			break;
		case 0x52: // LD D,D
			regs.D = regs.D;
			break;
		case 0x53: // LD D,E
			regs.D = regs.E;
			break;
		case 0x54: // LD D,H
			regs.D = regs.H;
			break;
		case 0x55: // LD D,L
			regs.D = regs.L;
			break;
		case 0x56: // LD D,(HL)
			regs.D = memory[regs.HL];
			break;

		case 0x5F: // LD E,A
			regs.E = regs.A;
			break;
		case 0x58: // LD E,B
			regs.E = regs.D;
			break;
		case 0x59: // LD E,C
			regs.E = regs.C;
			break;
		case 0x5A: // LD E,D
			regs.E = regs.D;
			break;
		case 0x5B: // LD E,E
			regs.E = regs.E;
			break;
		case 0x5C: // LD E,H
			regs.E = regs.H;
			break;
		case 0x5D: // LD E,L
			regs.E = regs.L;
			break;
		case 0x5E: // LD E,(HL)
			regs.E = memory[regs.HL];
			break;

		case 0x67: // LD H,A
			regs.H = regs.A;
			break;
		case 0x60: // LD H,B
			regs.H = regs.B;
			break;
		case 0x61: // LD H,C
			regs.H = regs.C;
			break;
		case 0x62: // LD H,D
			regs.H = regs.D;
			break;
		case 0x63: // LD H,E
			regs.H = regs.E;
			break;
		case 0x64: // LD H,H
			regs.H = regs.H;
			break;
		case 0x65: // LD H,L
			regs.H = regs.L;
			break;
		case 0x66: // LD H,(HL)
			regs.H = memory[regs.HL];
			break;

		case 0x6F: // LD L,A
			regs.L = regs.A;
			break;
		case 0x68: // LD L,B
			regs.L = regs.D;
			break;
		case 0x69: // LD L,C
			regs.L = regs.C;
			break;
		case 0x6A: // LD L,D
			regs.L = regs.D;
			break;
		case 0x6B: // LD L,E
			regs.L = regs.E;
			break;
		case 0x6C: // LD L,H
			regs.L = regs.H;
			break;
		case 0x6D: // LD L,L
			regs.L = regs.L;
			break;
		case 0x6E: // LD L,(HL)
			regs.L = memory[regs.HL];
			break;

		case 0x77: // LD (HL),A
			memory[regs.HL] = regs.A;
			break;
		case 0x70: // LD (HL),B
			memory[regs.HL] = regs.B;
			break;
		case 0x71: // LD (HL),C
			memory[regs.HL] = regs.C;
			break;
		case 0x72: // LD (HL),D
			memory[regs.HL] = regs.D;
			break;
		case 0x73: // LD (HL),E
			memory[regs.HL] = regs.E;
			break;
		case 0x74: // LD (HL),H
			memory[regs.HL] = regs.H;
			break;
		case 0x75: // LD (HL),L
			memory[regs.HL] = regs.L;
			break;
		case 0x36: // LD (HL), n
			memory[regs.HL] = memory[regs.PC++];
			break;
		
		// LD A, n
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
		
		// LD n, A
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
			if (res > 0xFFFF) set_flag(CARRY_FLAG);
			else reset_flag(CARRY_FLAG);

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
		case 0xF5: // push AF
			memory[regs.SP-1] = regs.A;
			memory[regs.SP-2] = regs.flags;
			regs.SP -= 2;
			break;
		case 0xC5: // push BC
			memory[regs.SP-1] = regs.B;
			memory[regs.SP-2] = regs.C;
			regs.SP -= 2;
			break;
		case 0xD5: // push DE
			memory[regs.SP-1] = regs.D;
			memory[regs.SP-2] = regs.E;
			regs.SP -= 2;
			break;
		case 0xE5: // push HL
			memory[regs.SP-1] = regs.H;
			memory[regs.SP-2] = regs.L;
			regs.SP -= 2;
			break;

		// POP nn
		case 0xF1: // pop AF
			regs.flags = memory[regs.SP];
			regs.A = memory[regs.SP+1];
			regs.SP += 2;
			break;
		case 0xC1: // pop BC
			regs.C = memory[regs.SP];
			regs.B = memory[regs.SP+1];
			regs.SP += 2;
			break;
		case 0xD1: // pop DE
			regs.E = memory[regs.SP];
			regs.D = memory[regs.SP+1];
			regs.SP += 2;
			break;
		case 0xE1: // pop HL
			regs.L = memory[regs.SP];
			regs.H = memory[regs.SP+1];
			regs.SP += 2;
			break;

		// 8-bit ALU
		// ADD A, n
		case 0x87: {// ADD A, A
			int res = regs.A + regs.A;
			int half_res = (regs.A & 0x0F) + (regs.A & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0x80: {// ADD A, B
			int res = regs.A + regs.B;
			int half_res = (regs.A & 0x0F) + (regs.B & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0x81: {// ADD A, C
			int res = regs.A + regs.C;
			int half_res = (regs.A & 0x0F) + (regs.C & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0x82: {// ADD A, D
			int res = regs.A + regs.D;
			int half_res = (regs.A & 0x0F) + (regs.D & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0x83: {// ADD A, E
			int res = regs.A + regs.E;
			int half_res = (regs.A & 0x0F) + (regs.E & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0x84: {// ADD A, H
			int res = regs.A + regs.H;
			int half_res = (regs.A & 0x0F) + (regs.H & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0x85: {// ADD A, L
			int res = regs.A + regs.L;
			int half_res = (regs.A & 0x0F) + (regs.L & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0x86: {// ADD A, (HL)
			int res = regs.A + memory[regs.HL];
			int half_res = (regs.A & 0x0F) + (memory[regs.HL] & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}
		case 0xC6: {//ADD A, #
			int inm = memory[regs.PC++];
			int res = regs.A + inm;
			int half_res = (regs.A & 0x0F) + (inm & 0x0F);
			
			regs.A = static_cast<u8>(res);
			if (res > 0xFF)
				set_flag(CARRY_FLAG);
			reset_flag(ADD_SUB_FLAG);
			if (regs.A == 0)
				set_flag(ZERO_FLAG);

			if (half_res > 0x0F)
				set_flag(HALF_CARRY_FLAG);
			break;
			}





			



	}

}

