#include "gbcore.h"

#include "GBRom.h"
#include "MBC.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>

GameBoy::GameBoy(std::string rom_name, GameBoyType type):
	gameboy_type(type),
	memory(this),
	video(),
	rom(0),
	regs(),
	IME(1),
	HALT(0),
	cycle_count(0)
{
	logger.info("GameBoy init");
	rom = read_gbrom(rom_name);

	MBC *mbc = create_MBC(rom);
	memory.init(mbc);
	
	reset();
}

void GameBoy::reset()
{
	logger.info("GameBoy reset");
	regs.PC = 0x0100;
	regs.AF = 0x01B0;
	regs.BC = 0x0013;
	regs.DE = 0x00D8;
	regs.HL = 0x014D;
	regs.SP = 0xFFFE;

	memory.write(0xFF05, 0x00);   // TIMA
	memory.write(0xFF06, 0x00);   // TMA
	memory.write(0xFF07, 0x00);   // TAC
	memory.write(0xFF10, 0x80);   // NR10
	memory.write(0xFF11, 0xBF);   // NR11
	memory.write(0xFF12, 0xF3);   // NR12
	memory.write(0xFF14, 0xBF);   // NR14
	memory.write(0xFF16, 0x3F);   // NR21
	memory.write(0xFF17, 0x00);   // NR22
	memory.write(0xFF19, 0xBF);   // NR24
	memory.write(0xFF1A, 0x7F);   // NR30
	memory.write(0xFF1B, 0xFF);   // NR31
	memory.write(0xFF1C, 0x9F);   // NR32
	memory.write(0xFF1E, 0xBF);   // NR33
	memory.write(0xFF20, 0xFF);   // NR41
	memory.write(0xFF21, 0x00);   // NR42
	memory.write(0xFF22, 0x00);   // NR43
	memory.write(0xFF23, 0xBF);   // NR30
	memory.write(0xFF24, 0x77);   // NR50
	memory.write(0xFF25, 0xF3);   // NR51
	// NR52
	if (gameboy_type == SUPERGAMEBOY)
		memory.write(0xFF26, 0xF0);
	else
		memory.write(0xFF26, 0xF1);
	memory.write(0xFF40, 0x91);   // LCDC
	memory.write(0xFF42, 0x00);   // SCY
	memory.write(0xFF43, 0x00);   // SCX
	memory.write(0xFF45, 0x00);   // LYC
	memory.write(0xFF47, 0xFC);   // BGP
	memory.write(0xFF48, 0xFF);   // OBP0
	memory.write(0xFF49, 0xFF);   // OBP1
	memory.write(0xFF4A, 0x00);   // WY
	memory.write(0xFF4B, 0x00);   // WX
	memory.write(0xFFFF, 0x00);   // IE
}

#include "opcodes.h"

void GameBoy::run_cycle()
{
	// Check for interrupts before opcode fetching
	u8 IE;
	if (IME && (IE=memory.read(0xFFFF)))
	{
		u8 IF = memory.read(0xFF0F);
		if (IF)
		{
			if ((IF & IRQ_VBLANK) && (IE & IRQ_VBLANK)) 
			{
				IME = 0;
				IF &= (~IRQ_VBLANK);
				do_call(0x40);
			}
			else if ((IF & IRQ_LCD_STAT) && (IE & IRQ_LCD_STAT))
			{
				IME = 0;
				IF &= (~IRQ_LCD_STAT);
				do_call(0x48);
			} 
			else if ((IF & IRQ_TIMER) && (IE & IRQ_TIMER))
			{
				IME = 0;
				IF &= (~IRQ_TIMER);
				do_call(0x50);
			}
			else if ((IF & IRQ_SERIAL) && (IE & IRQ_SERIAL))   
			{
				IME = 0;
				IF &= (~IRQ_SERIAL);
				do_call(0x58);
			}
			else if ((IF & IRQ_JOYPAD) && (IE & IRQ_JOYPAD))     
			{
				IME = 0;
				IF &= (~IRQ_JOYPAD);
				do_call(0x60);
			}
		}
	}



	int prefix;
	int opcode;
	opcode = memory.read(regs.PC++);
	if (opcode == 0xCB)
	{
		prefix=opcode;
		opcode=memory.read(regs.PC++);
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
			memory.write(regs.HL, memory.read(regs.PC++));
			break;
		
		// LD A, mem
		case 0x0A: // LD A, (BC)
			regs.A = memory.read(regs.BC);
			break;
		case 0x1A: // LD A, (DE)
			regs.A = memory.read(regs.DE);
			break;
		case 0xFA: // LD A, (nn)
			regs.A = memory.read(memory.read(regs.PC) + memory.read(regs.PC+1)<<8);
			regs.PC+=2;
			break;
		
		// LD mem, A
		case 0x02: // LD (BC), A
			memory.write(regs.BC, regs.A);
			break;
		case 0x12: // LD (DE), A
			memory.write(regs.DE, regs.A);
			break;
		case 0xEA: // LD (nn), A
			memory.write(memory.read(regs.PC) + memory.read(regs.PC+1)<<8, regs.A);
			regs.PC+=2;
			break;

		// LD A, (C)
		case 0xF2:
			regs.A = memory.read(0xFF00 + regs.C);
			break;
		// LD (C), A
		case 0xE2:
			memory.write(0xFF00 + regs.C, regs.A);
			break;

		// LD A, (HLD); LD A, (HL-); LDD A,(HL);
		case 0x3A:
			regs.A = memory.read(regs.HL);
			--regs.HL;
			break;
		// LD (HLD), A; LD (HL-), A; LDD (HL), A;
		case 0x32:
			memory.write(regs.HL, regs.A);
			--regs.HL;
			break;
		// LD A, (HLI); LD A, (HL+); LDI A, (HL);
		case 0x2A:
			regs.A = memory.read(regs.HL);
			++regs.HL;
			break;
		// LD (HLI), A; LD (HL+), A; LDI (HL), A;
		case 0x22:
			memory.write(regs.HL, regs.A);
			++regs.HL;
			break;

		// LDH (n), A
		case 0xE0: {
			memory.write(0xFF00 + memory.read(regs.PC++), regs.A);
			break;
		}
		// LDH A, (n)
		case 0xF0:
			regs.A = memory.read(0xFF00 + memory.read(regs.PC++));
			break;

		// LD n, nn
		case 0x01: // LD BC, nn
			regs.BC = memory.read(regs.PC)+(memory.read(regs.PC+1) << 8);
			regs.PC +=2;
			break;
		case 0x11: // LD DE, nn
			regs.DE = memory.read(regs.PC)+(memory.read(regs.PC+1) << 8);
			regs.PC +=2;
			break;
		case 0x21: // LD HL, nn
			regs.HL = memory.read(regs.PC)+(memory.read(regs.PC+1) << 8);
			regs.PC +=2;
			break;
		case 0x31: // LD SP, nn
			regs.SP = memory.read(regs.PC)+(memory.read(regs.PC+1) << 8);
			regs.PC +=2;
			break;
		
		// LD SP, HL
		case 0xF9:
			regs.SP = regs.HL;
			break;

		// LD HL, SP+n
		// LDHL SP, n
		case 0xF8: {
			s8 offset = memory.read(regs.PC++);
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
			int addr = memory.read(regs.PC) + memory.read(regs.PC+1) << 8;
			regs.PC += 2;
			memory.write(addr, regs.SP);
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
			int res = regs.A + memory.read(regs.HL);
			int half_res = (regs.A & 0x0F) + (memory.read(regs.HL) & 0x0F);
			regs.A = static_cast<u8>(res);
			
			reset_flag(ADD_SUB_FLAG);
			set_flag_if (res > 0xFF,      CARRY_FLAG);
			set_flag_if (regs.A == 0,     ZERO_FLAG);
			set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG);
			break;
			}
		case 0xC6: {//ADD A, #
			int inm = memory.read(regs.PC++);
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
			int res = regs.A + memory.read(regs.HL) + carry;
			int half_res = (regs.A & 0x0F) + (memory.read(regs.HL) & 0x0F) + carry;
			regs.A = static_cast<u8>(res);
			
			reset_flag(ADD_SUB_FLAG);
			set_flag_if (res > 0xFF,      CARRY_FLAG);
			set_flag_if (regs.A == 0,     ZERO_FLAG);
			set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG);
			break;
			}
		case 0xCE: {//ADC A, #
			int carry = (check_flag(CARRY_FLAG)? 1 : 0);
			int inm = memory.read(regs.PC++);
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
			int res = regs.A - memory.read(regs.HL);
			int half_res = (regs.A & 0x0F) - (memory.read(regs.HL) & 0x0F);
			regs.A = static_cast<u8>(res);

			set_flag(ADD_SUB_FLAG);
			set_flag_if (res < 0,      CARRY_FLAG);
			set_flag_if (res == 0,     ZERO_FLAG);
			set_flag_if (half_res < 0, HALF_CARRY_FLAG);
			break;
			}
		
		case 0xD6: {//SUB #
			int inm = memory.read(regs.PC++);
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
			int res = regs.A - memory.read(regs.HL) - carry;
			int half_res = (regs.A & 0x0F) - (memory.read(regs.HL) & 0x0F) - carry;
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
			regs.A &= memory.read(regs.HL);
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			set_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		case 0xE6: //AND inm
			regs.A &= memory.read(regs.PC++);
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			set_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		// OR n
		for_each_register(0xB7, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, OR_reg)

		case 0xB6: //OR (HL)
			regs.A |= memory.read(regs.HL);
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		case 0xF6: //OR inm
			regs.A |= memory.read(regs.PC++);
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		// XOR n
		for_each_register(0xAF, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, XOR_reg)

		case 0xAE: //XOR (HL)
			regs.A ^= memory.read(regs.HL);
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;

		case 0xEE: //XOR inm
			regs.A ^= memory.read(regs.PC++);
			if (regs.A == 0) set_flag(ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			reset_flag(CARRY_FLAG);
			break;
		
		// CP n
		for_each_register(0xBF, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, CP_reg)

		case 0xBE: {//SUB (HL)
			int res = regs.A - memory.read(regs.HL);
			int half_res = (regs.A & 0x0F) - (memory.read(regs.HL) & 0x0F);
			regs.A = static_cast<u8>(res);

			set_flag(ADD_SUB_FLAG);
			set_flag_if (res < 0,      CARRY_FLAG);
			set_flag_if (res == 0,     ZERO_FLAG);
			set_flag_if (half_res < 0, HALF_CARRY_FLAG);
			break;
			}
		
		case 0xFE: {//SUB #
			int inm = memory.read(regs.PC++);
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
			int half_res = (memory.read(regs.HL) & 0x0F) + 1; 
			memory.write(regs.HL, memory.read(regs.HL) - 1); 
			reset_flag(ADD_SUB_FLAG); 
			set_flag_if (memory.read(regs.HL) == 0, ZERO_FLAG); 
			set_flag_if (half_res > 0x0F,      HALF_CARRY_FLAG); 
			break; 
			}	

		// DEC n
		for_each_register(0x3D, 0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, DEC_reg)

		case 0x35: {//DEC (HL)
			int half_res = (memory.read(regs.HL) & 0x0F) - 1; 
			memory.write(regs.HL, memory.read(regs.HL) - 1); 
			set_flag(ADD_SUB_FLAG); 
			set_flag_if (memory.read(regs.HL) == 0, ZERO_FLAG); 
			set_flag_if (half_res < 0,         HALF_CARRY_FLAG);
			break; 
			}

		// 16-bit ALU
		// ADD HL, n
		for_each_register16(0x09, 0x19, 0x29, 0x39, ADD_HL_reg16)

		// ADD SP, #
		case 0xE8: {
			// FIXME: No se que hacer con el half carry, en 4 o en 11?
			int n = static_cast<s8>(memory.read(regs.PC++));
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
		case 0xCB: {
			int sub_opcode = memory.read(regs.PC++);
			switch(sub_opcode)
			{
				// SWAP n
				for_each_register(0x37, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, SWAP_reg)

				// SWAP (HL)
				case 0x36: {
					u8 tmp = memory.read(regs.HL);
					tmp = ((tmp & 0x0F) << 4) | ((tmp & 0xF0)>>4);
					memory.write(regs.HL, tmp);
		
					set_flag_if(tmp==0, ZERO_FLAG);
					reset_flag(CARRY_FLAG);
					reset_flag(HALF_CARRY_FLAG);
					reset_flag(ADD_SUB_FLAG);
					break;
				}

				// RLC n
				for_each_register(0x07, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, RLC_reg)

				// RLC (HL)
				case 0x06: {
					u8 value = memory.read(regs.HL);
					u8 bit7 = value >> 7;
					value = (value << 1) | bit7;
					memory.write(regs.HL, value);
					set_flag_if(value == 0, ZERO_FLAG); 
					reset_flag(ADD_SUB_FLAG); 
					reset_flag(HALF_CARRY_FLAG); 
					break;
				}

				// RL n (through carry)
				for_each_register(0x17, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, RL_reg)

				// RL (HL) (through carry)
				case 0x16: {
					u8 value = memory.read(regs.HL);
					u8 bit7 = value >> 7;
					value = (value << 1) | check_flag(CARRY_FLAG);
					memory.write(regs.HL, value);
					set_flag_if(bit7, CARRY_FLAG);
					set_flag_if(value == 0, ZERO_FLAG);
					reset_flag(ADD_SUB_FLAG); 
					reset_flag(HALF_CARRY_FLAG); 
					break;
				}

				// RRC n
				for_each_register(0x0F, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, RRC_reg)

				// RRC (HL)
				case 0x0E: {
					u8 value = memory.read(regs.HL);
					u8 bit0 = value & 1;
					value = (value >> 1) | (bit0 << 7);
					memory.write(regs.HL, value);
					set_flag_if(value == 0, ZERO_FLAG); 
					reset_flag(ADD_SUB_FLAG); 
					reset_flag(HALF_CARRY_FLAG); 
					break;
				}

				// RR n (through carry)
				for_each_register(0x1F, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, RR_reg)

				// RR (HL) (through carry)
				case 0x1E: {
					u8 value = memory.read(regs.HL);
					u8 bit0 = value & 1;
					value = (value >> 1) | (check_flag(CARRY_FLAG) << 7);
					memory.write(regs.HL, value);
					set_flag_if(bit0, CARRY_FLAG);
					set_flag_if(value == 0, ZERO_FLAG);
					reset_flag(ADD_SUB_FLAG); 
					reset_flag(HALF_CARRY_FLAG); 
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
		case 0x10: {
			int sub_opcode = memory.read(regs.PC++);
			if (sub_opcode == 0x00) {
				HALT = true;
			} else {
				logger.critical("Unknown sub-opcode after 0x10");
			}
			break;
		}
		
		// DI
		case 0xF3:
			IME = 0;
			break;

		// EI
		case 0xFB:
			IME = 1;
			break;
			   
		// Rotates and shifts
		// RLCA
		case 0x07: {
			u8 bit7 = regs.A >>7;
			regs.A = (regs.A << 1) | bit7;
			set_flag_if(regs.A == 0, ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			// TODO: Check which of GBCPUman.pdf or
			// worldofspectrum z80 reference is correct
			//
			//set_flag_if(bit7, CARRY_FLAG);
			break;
		}

		// RLA (through carry)
		case 0x17: {
			u8 bit7 = regs.A >> 7;
			regs.A = (regs.A << 1) | check_flag(CARRY_FLAG);
			set_flag_if(bit7, CARRY_FLAG);
			set_flag_if(regs.A == 0, ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			break;
		}
		
		// RRCA
		case 0x0F: {
			u8 bit0 = regs.A & 1;
			regs.A = (regs.A >> 1) | (bit0 << 7);
			set_flag_if(regs.A == 0, ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			// TODO: Check which of GBCPUman.pdf or
			// worldofspectrum z80 reference is correct
			//
			//set_flag_if(bit0, CARRY_FLAG);
			break;
		}

		// RRA (through carry)
		case 0x1F: {
			u8 bit0 = regs.A & 1;
			regs.A = (regs.A >> 1) | (check_flag(CARRY_FLAG) << 7);
			set_flag_if(bit0, CARRY_FLAG);
			set_flag_if(regs.A == 0, ZERO_FLAG);
			reset_flag(ADD_SUB_FLAG);
			reset_flag(HALF_CARRY_FLAG);
			break;
		}
		




		


		// TODO: Bit instructions
		
		// Jumps
		// JP nn
		case 0xC3:
			regs.PC = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
			break;

		// JP cc, nn
		case 0xC2: { // JP NZ, nn
			u16 dst = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
			if (!check_flag(ZERO_FLAG))
				regs.PC = dst;
			else
				regs.PC += 2; // if !cc, skip 2 dst bytes
			break;
		}

		case 0xCA: { // JP Z, nn
			u16 dst = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
			if (check_flag(ZERO_FLAG))
				regs.PC = dst;
			else
				regs.PC += 2; // if !cc, skip 2 dst bytes
			break;
		}

		case 0xD2: { // JP NC, nn
			u16 dst = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
			if (!check_flag(CARRY_FLAG))
				regs.PC = dst;
			else
				regs.PC += 2; // if !cc, skip 2 dst bytes
			break;
		}

		case 0xDA: { // JP C, nn
			u16 dst = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
			if (check_flag(CARRY_FLAG))
				regs.PC = dst;
			else
				regs.PC += 2; // if !cc, skip 2 dst bytes
			break;
		}

		// JP (HL)
		case 0xE9:
			regs.PC = regs.HL;
			break;

		// JR n
		case 0x18:
			// -1 because PC is now pointing past the opcode
			regs.PC += static_cast<s8>(memory.read(regs.PC)) - 1;
			break;

		// JR cc, n
		case 0x20: { // JR NZ, n
			s8 offset = static_cast<s8>(memory.read(regs.PC++));
			if (!check_flag(ZERO_FLAG)) // -1 because PC is now pointing past the opcode
				regs.PC += offset;
			break;
		}

		case 0x28: { // JR Z, n
			s8 offset = static_cast<s8>(memory.read(regs.PC++));
			if (check_flag(ZERO_FLAG)) // -1 because PC is now pointing past the opcode
				regs.PC += offset;
			break;
		}

		case 0x30: { // JR NC, n
			s8 offset = static_cast<s8>(memory.read(regs.PC++));
			if (!check_flag(CARRY_FLAG)) // -1 because PC is now pointing past the opcode
				regs.PC += offset;
			break;
		}

		case 0x38: { // JR C, n
			s8 offset = static_cast<s8>(memory.read(regs.PC++));
			if (check_flag(CARRY_FLAG)) // -1 because PC is now pointing past the opcode
				regs.PC += offset;
			break;
		}

		// Calls
		// CALL nn
		case 0xCD: {
			u16 addr = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
			regs.PC += 2;
			do_call(addr);
			break;
		}

		// CALL cc, nn
		case 0xC4: { // CALL NZ, nn
			if (!check_flag(ZERO_FLAG)) {
				u16 addr = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
				regs.PC += 2;
				do_call(addr);
			} else {
				regs.PC += 2; // if !cc, skip 2 (nn) bytes
			}
		}

		case 0xCC: { // CALL Z, nn
			if (check_flag(ZERO_FLAG)) {
				u16 addr = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
				regs.PC += 2;
				do_call(addr);
			} else {
				regs.PC += 2; // if !cc, skip 2 (nn) bytes
			}
		}

		case 0xD4: { // CALL NC, nn
			if (!check_flag(CARRY_FLAG)) {
				u16 addr = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
				regs.PC += 2;
				do_call(addr);
			} else {
				regs.PC += 2; // if !cc, skip 2 (nn) bytes
			}
		}

		case 0xDC: { // CALL C, nn
			if (check_flag(CARRY_FLAG)) {
				u16 addr = memory.read(regs.PC) | (memory.read(regs.PC+1)<<8);
				regs.PC += 2;
				do_call(addr);
			} else {
				regs.PC += 2; // if !cc, skip 2 (nn) bytes
			}
		}

		// Restarts
		RST(0xC7, 0x00)
		RST(0xCF, 0x08)
		RST(0xD7, 0x10)
		RST(0xDF, 0x18)
		RST(0xE7, 0x20)
		RST(0xEF, 0x28)
		RST(0xF7, 0x30)
		RST(0xFF, 0x38)

		// Returns
		// RET
		case 0xC9: {
			u16 retaddr = (memory.read(regs.SP+1)<<8) | memory.read(regs.SP);
			regs.SP += 2;
			regs.PC = retaddr;
		}

		// RET cc
		case 0xC0:  // RET NZ
			if (!check_flag(ZERO_FLAG)) { 
				u16 retaddr = (memory.read(regs.SP+1)<<8) | memory.read(regs.SP);
				regs.SP += 2;
				regs.PC = retaddr;
			}
			break;

		case 0xC8:  // RET Z
			if (check_flag(ZERO_FLAG)) { 
				u16 retaddr = (memory.read(regs.SP+1)<<8) | memory.read(regs.SP);
				regs.SP += 2;
				regs.PC = retaddr;
			}
			break;

		case 0xD0:  // RET NC
			if (!check_flag(CARRY_FLAG)) { 
				u16 retaddr = (memory.read(regs.SP+1)<<8) | memory.read(regs.SP);
				regs.SP += 2;
				regs.PC = retaddr;
			}
			break;

		case 0xD8:  // RET C
			if (check_flag(CARRY_FLAG)) { 
				u16 retaddr = (memory.read(regs.SP+1)<<8) | memory.read(regs.SP);
				regs.SP += 2;
				regs.PC = retaddr;
			}
			break;

		// RETI
		case 0xD9: {
			// RET && EI
			u16 retaddr = (memory.read(regs.SP+1)<<8) | memory.read(regs.SP);
			regs.SP += 2;
			regs.PC = retaddr;
			IME=1;
		}
	
		default:
			std::ostringstream errmsg;
			errmsg << "Unknown opcode 0x";
			errmsg << std::hex << std::setw(2) << std::setfill('0') << opcode;
			errmsg << " at 0x" << std::hex << std::setw(4) << regs.PC-1;
			errmsg << " (cycle count = " << std::dec << cycle_count << ")";
			logger.critical(errmsg.str());
			break;

	} // end switch

	std::ostringstream tracemsg;
	tracemsg << "t = " << std::dec << cycle_count << 
		"\tPC = " << std::hex << std::setw(4) << std::setfill('0') << regs.PC << std::endl <<
		"A = " << std::hex << std::setw(2) << std::setfill('0') << int(regs.A) <<
		" B = " << std::hex << std::setw(2) << std::setfill('0') << int(regs.B) <<
		" C = " << std::hex << std::setw(2) << std::setfill('0') << int(regs.C) <<
		" D = " << std::hex << std::setw(2) << std::setfill('0') << int(regs.D) <<
		" E = " << std::hex << std::setw(2) << std::setfill('0') << int(regs.E) <<
		" H = " << std::hex << std::setw(2) << std::setfill('0') << int(regs.H) <<
		" L = " << std::hex << std::setw(2) << std::setfill('0') << int(regs.L) <<
		"\tflags = " << int(regs.flags) << "\tZF = " << check_flag(ZERO_FLAG);
	logger.trace(tracemsg.str());
	++cycle_count;

}

void GameBoy::run() 
{
}



