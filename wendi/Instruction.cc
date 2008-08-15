#include "Instruction.h"

#if 0
		// Jumps
		dis_inm16(0xC3, "JP")
		// JP cc, nn
		dis_inm16(0xC2, "JP NZ")
		dis_inm16(0xCA, "JP Z")
		dis_inm16(0xD2, "JP NC")
		dis_inm16(0xDA, "JP C")
		case 0xE9:
			result << "JP (HL)";
			opcode_str = "JP";
			op1.str="(HL)";
			op2.str="";
			op1.type = Instruction::MEM_INDIRECT;
			op1.reg = Instruction::HL;
			op2.type = Instruction::NONE;
			break;

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
#endif
