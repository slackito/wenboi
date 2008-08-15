#include "disasm.h"
#include "disasm_macros.h"

std::string get_port_name(int port)
{
	std::string port_name;

	switch (port)
	{
		case 0x04: port_name = "DIV "; break;
		case 0x05: port_name = "TIMA"; break;
		case 0x06: port_name = "TMA "; break;
		case 0x07: port_name = "TAC "; break;
		case 0x10: port_name = "CH1_ENT";          break;
		case 0x11: port_name = "CH1_WAVE";         break;
		case 0x12: port_name = "CH1_ENV";          break;
		case 0x13: port_name = "CH1_FREQ_LO";      break;
		case 0x14: port_name = "CH1_FREQ_HI_KICK"; break;
		case 0x16: port_name = "CH2_WAVE";         break;
		case 0x17: port_name = "CH2_ENV";          break;
		case 0x18: port_name = "CH2_FREQ_LO";      break;
		case 0x19: port_name = "CH2_FREQ_HI_KICK"; break;
		case 0x1A: port_name = "CH3_ONOFF";        break;
		case 0x1C: port_name = "CH3_VOLUME";       break;
		case 0x1D: port_name = "CH3_FREQ_LO";      break;
		case 0x1E: port_name = "CH3_FREQ_HI_KICK"; break;
		case 0x21: port_name = "CH4_ENV";          break;
		case 0x22: port_name = "CH4_POLY";         break;
		case 0x23: port_name = "CH4_KICK";         break;
		case 0x24: port_name = "SND_VIN";          break;
		case 0x25: port_name = "SND_STEREO";       break;
		case 0x26: port_name = "SND_STAT";         break;
		case 0x40: port_name = "LCDC"; break;
		case 0x41: port_name = "STAT"; break;
		case 0x42: port_name = "SCY "; break; 
		case 0x43: port_name = "SCX "; break; 
		case 0x44: port_name = "LY  "; break; 
		case 0x45: port_name = "LYC "; break; 
		case 0x4A: port_name = "WY  "; break; 
		case 0x4B: port_name = "WX  "; break; 
		case 0x47: port_name = "BGP "; break; 
		case 0x48: port_name = "OBP0"; break; 
		case 0x49: port_name = "OBP1"; break; 
		case 0x46: port_name = "DMA "; break;
		case 0x0F: port_name = "IF  "; break;
		case 0xFF: port_name = "IE  "; break;
		default:
				   if (port >= 0x80 && port <= 0xFE) 
					   port_name = "HRAM";
				   else if (port >= 0x30 && port <= 0x3F)
					   port_name = "Wave Pattern RAM";

	}
	return port_name;
}


void disassemble_opcode(GameBoy &gb, u16 addr, std::string &instruction, int &length)
{
	int opcode;
	std::ostringstream result;
	std::string              opcode_str, op1_str, op2_str;
	Instruction::OperandType op1_type, op2_type;
	Instruction::Operand     op1, op2;

	u16 PC = addr;
	opcode = gb.memory.read(PC++, GBMemory::DONT_WATCH);

	result << std::hex << std::uppercase << std::setfill('0');
	
	switch(opcode)
	{
		// LD n, nn
		dis_for_each_register(0x3E, 0x06, 0x0E, 0x16, 0x1E, 0x26, 0x2E, "LD", dis_reg_inm)

		// LD r1,r2
		dis_for_each_register(0x7F, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, "LD", dis_A_reg)
		dis_for_each_register(0x47, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, "LD", dis_B_reg)
		dis_for_each_register(0x4F, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, "LD", dis_C_reg)
		dis_for_each_register(0x57, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, "LD", dis_D_reg)
		dis_for_each_register(0x5F, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, "LD", dis_E_reg)
		dis_for_each_register(0x67, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, "LD", dis_H_reg)
		dis_for_each_register(0x6F, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, "LD", dis_L_reg)
		
		// LD reg, (HL)
		dis_for_each_register(0x7E, 0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E, "LD", dis_reg__HL_)

		// LD (HL), reg
		dis_for_each_register(0x77, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, "LD", dis__HL__reg)
	
		dis__reg16__inm(0x36, "LD", HL)
		
		dis_reg__reg16_(0x0A, "LD", A, BC)
		dis_reg__reg16_(0x1A, "LD", A, DE)
		dis_reg__inm_(0xFA, "LD", A)

		dis__reg16__reg(0x02, "LD", BC, A)
		dis__reg16__reg(0x12, "LD", DE, A)
		dis__inm__reg(0xEA, "LD", A)

		// LD A, (C)
		dis(0xF2, "LD A, (0xFF00+C)")
		// LD (C), A
		dis(0xE2, "LD (0xFF00+C), A")

		// LD A, (HLD); LD A, (HL-); LDD A,(HL);
		dis(0x3A, "LDD A, (HL)")
		// LD (HLD), A; LD (HL-), A; LDD (HL), A;
		dis(0x32, "LDD (HL), A")
		// LD A, (HLI); LD A, (HL+); LDI A, (HL);
		dis(0x2A, "LDI A, (HL)")
		// LD (HLI), A; LD (HL+), A; LDI (HL), A;
		dis(0x22, "LDI (HL), A")

		// LDH (n), A
		case 0xE0: {
			int port = int(gb.memory.read(PC++, GBMemory::DONT_WATCH));
			
			result << "LD (0xFF" << 
					std::setw(2) << port << "), A" << "\t[" << get_port_name(port) << "]";
			break;
		}
		// LDH A, (n)
		case 0xF0: {
			int port = int(gb.memory.read(PC++, GBMemory::DONT_WATCH));
			result << "LD A, (0xFF" << 
					std::setw(2) << port << ")" << "\t[" << get_port_name(port) << "]";
			break;
		}

		dis_reg16_inm(0x01, "LD", BC)
		dis_reg16_inm(0x11, "LD", DE)
		dis_reg16_inm(0x21, "LD", HL)
		dis_reg16_inm(0x31, "LD", SP)
		
		// LD SP, HL
		dis(0xF9, "LD SP, HL")

		// LD HL, SP+n
		// LDHL SP, n
		case 0xF8: 
			result << "LD HL, SP + 0x"<< std::setw(2) << int(gb.memory.read(PC++, GBMemory::DONT_WATCH));
			break; 

		// LD (nn), SP
		dis__inm__reg16(0x08, "LD", SP)

		// PUSH nn
		dis_reg16(0xF5, "PUSH", AF)
		dis_reg16(0xC5, "PUSH", BC)
		dis_reg16(0xD5, "PUSH", DE)
		dis_reg16(0xE5, "PUSH", HL)

		// POP nn
		dis_reg16(0xF1, "POP", AF)
		dis_reg16(0xC1, "POP", BC)
		dis_reg16(0xD1, "POP", DE)
		dis_reg16(0xE1, "POP", HL)

		// 8-bit ALU
		// ADD A,reg
		dis_for_each_register(0x87, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, "ADD", dis_A_reg)

		dis_reg__reg16_(0x86, "ADD", A, HL)
		dis_reg_inm(0xC6, "ADD", A)
		
		// ADC A, n
		dis_for_each_register(0x8F, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, "ADC", dis_A_reg)
		
		dis_reg__reg16_(0x8E, "ADC", A, HL)
		dis_reg_inm(0xCE, "ADC", A)

		// SUB n
		dis_for_each_register(0x97, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, "SUB", dis_reg)
		
		dis__reg16_(0x96, "SUB", HL)
		dis_inm8(0xD6, "SUB")	
		
		// SBC n
		dis_for_each_register(0x9F, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, "SBC", dis_reg)

		dis__reg16_(0x9E, "SBC", HL)

		// AND n
		dis_for_each_register(0xA7, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, "AND", dis_reg)

		dis__reg16_(0xA6, "AND", HL)
		dis_inm8(0xE6, "AND")
		
		// OR n
		dis_for_each_register(0xB7, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, "OR", dis_reg)
		
		dis__reg16_(0xB6, "OR", HL)
		dis_inm8(0xF6, "OR")

		// XOR n
		dis_for_each_register(0xAF, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, "XOR", dis_reg)

		dis__reg16_(0xAE, "XOR", HL)
		dis_inm8(0xEE, "XOR")
		
		// CP n
		dis_for_each_register(0xBF, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, "CP", dis_reg)
		
		dis__reg16_(0xBE, "CP", HL)
		dis_inm8(0xFE, "CP")

		// INC n
		dis_for_each_register(0x3C, 0x04, 0x0C, 0x14, 0x1C, 0x24, 0x2C, "INC", dis_reg)

		dis__reg16_(0x34, "INC", HL)
		
		// DEC n
		dis_for_each_register(0x3D, 0x05, 0x0D, 0x15, 0x1D, 0x25, 0x2D, "DEC", dis_reg)

		dis__reg16_(0x35, "DEC", HL)
		
		// 16-bit ALU
		// ADD HL, n
		dis_for_each_register16(0x09, 0x19, 0x29, 0x39, "ADD", dis_HL_reg16)

		// ADD SP, #
		dis_reg16_inm8(0xE8, "ADD", SP)
		
		// INC nn
		dis_for_each_register16(0x03, 0x13, 0x23, 0x33, "INC", dis_reg16)

		// DEC nn
		dis_for_each_register16(0x0B, 0x1B, 0x2B, 0x3B, "DEC", dis_reg16)

		// Miscellaneous instructions
		case 0xCB: {
			int sub_opcode = gb.memory.read(PC++, GBMemory::DONT_WATCH);
			switch(sub_opcode)
			{
				// SWAP n
				dis_for_each_register(0x37, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, "SWAP", dis_reg)

				// SWAP (HL)
				dis__reg16_(0x36, "SWAP", HL)

				// RLC n
				dis_for_each_register(0x07, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, "RLC", dis_reg)

				// RLC (HL)
				dis__reg16_(0x06, "RLC", HL)

				// RL n (through carry)
				dis_for_each_register(0x17, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, "RL", dis_reg)

				// RL (HL) (through carry)
				dis__reg16_(0x16, "RL", HL)

				// RRC n
				dis_for_each_register(0x0F, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, "RRC", dis_reg)

				// RRC (HL)
				dis__reg16_(0x0E, "RRC", HL)

				// RR n (through carry)
				dis_for_each_register(0x1F, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, "RR", dis_reg)

				// RR (HL) (through carry)
				dis__reg16_(0x1E, "RR", HL)

				// SLA n
				dis_for_each_register(0x27, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, "SLA", dis_reg)

				// SLA (HL)
				dis__reg16_(0x26, "SLA", HL)

				// SRA n
				dis_for_each_register(0x2F, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, "SRA", dis_reg)
				
				// SRA (HL)
				dis__reg16_(0x2E, "SRA", HL)

				// SRL n
				dis_for_each_register(0x3F, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, "SRA", dis_reg)
				
				// SRL (HL)
				dis__reg16_(0x3E, "SRA", HL)

				default: {
					int bit_op = sub_opcode >> 6;
					int reg = sub_opcode & 7;
					int b   = (sub_opcode >> 3) & 7;
					const char *bit_ops[4]={"Unknown", "BIT", "RES", "SET"};
					const char *regs[8]={"B","C","D","E","H","L","(HL)", "A"};
					result << bit_ops[bit_op] << " " << b << ", " << regs[reg];
					break;
				}
				
			}
			break;
		}

		dis(0x27, "DAA")

		dis(0x2F, "CPL")

		dis(0x3F, "CCF")

		dis(0x37, "SCF")
		dis(0x00, "NOP")
		dis(0x76, "HALT")

		dis(0x10, "STOP")
		
		dis(0xF3, "DI")

		dis(0xFB, "EI")
			   
		// Rotates and shifts
		dis(0x07, "RLCA")

		dis(0x17, "RLA")
		
		dis(0x0F, "RRCA")

		dis(0x1F, "RRA")
		
		// TODO: Bit instructions
		
		// Jumps
		dis_inm16(0xC3, "JP")
		// JP cc, nn
		dis_inm16(0xC2, "JP NZ")
		dis_inm16(0xCA, "JP Z")
		dis_inm16(0xD2, "JP NC")
		dis_inm16(0xDA, "JP C")
		dis(0xE9, "JP (HL)")
		
		dis_JR(0x18, "JR")
		dis_JR(0x20, "JR NZ")
		dis_JR(0x28, "JR Z")
		dis_JR(0x30, "JR NC")
		dis_JR(0x38, "JR C")

		// Calls
		dis_inm16(0xCD, "CALL")
		// CALL cc, nn
		dis_inm16(0xC4, "CALL NZ")
		dis_inm16(0xCC, "CALL Z")
		dis_inm16(0xD4, "CALL NC")
		dis_inm16(0xDC, "CALL C")

		// Restarts
		dis(0xC7, "RST 0x00")
		dis(0xCF, "RST 0x08")
		dis(0xD7, "RST 0x10")
		dis(0xDF, "RST 0x18")
		dis(0xE7, "RST 0x20")
		dis(0xEF, "RST 0x28")
		dis(0xF7, "RST 0x30")
		dis(0xFF, "RST 0x38")

		// Returns
		dis(0xC9, "RET")
		// RET cc
		dis(0xC0, "RET NZ")
		dis(0xC8, "RET Z")
		dis(0xD0, "RET NC")
		dis(0xD8, "RET C")

		dis(0xD9, "RETI")

		default:
			std::ostringstream errmsg;
			errmsg << "Unknown opcode 0x";
			errmsg << std::hex << std::setw(2) << std::setfill('0') << opcode;
			errmsg << " at 0x" << std::hex << std::setw(4) << PC-1;
			logger.trace(errmsg.str());
			break;

	} // end switch
	
	instruction = result.str();
	length = PC - addr;
}

