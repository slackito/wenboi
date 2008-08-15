#ifndef DISASM_MACROS_H
#define DISASM_MACROS_H

#define dis_for_each_register(opA, opB, opC, opD, opE, opH, opL, name, macro) \
	macro(opA, name, A) \
	macro(opB, name, B) \
    macro(opC, name, C) \
    macro(opD, name, D) \
	macro(opE, name, E) \
	macro(opH, name, H) \
	macro(opL, name, L)

#define dis_for_each_register16(opBC, opDE, opHL, opSP, name, macro) \
	macro(opBC, name, BC) \
	macro(opDE, name, DE) \
	macro(opHL, name, HL) \
	macro(opSP, name, SP) 

#define dis(opcode, name) \
	case opcode: \
		result << name; \
		break;

// OP reg
#define dis_reg(opcode, name, reg) \
	case opcode: \
		result << name << " " << #reg; \
		opcode_str = name; \
		op1_str  = #reg; \
		op2_str  = ""; \
		op1_type = REG; \
		op1.reg  = reg; \
		op2.type = NONE; \
		break;


// OP reg16
#define dis_reg16(opcode, name, reg16) \
	case opcode: \
		result << name << " " << #reg16; \
		opcode_str = name; \
		op1_str  = #reg16; \
		op2_str  = ""; \
		op1_type = REG; \
		op1.reg  = reg16; \
		op2.type = NONE; \
		break;

// OP (reg16)
#define dis__reg16_(opcode, name, reg16) \
	case opcode: \
		result << name << " (" << #reg16 << ")"; \
		opcode_str = name; \
		op1_str  = std::string("(")+#reg16+")"; \
		op2_str  = ""; \
		op1_type = MEM_INDIRECT; \
		op1.reg  = reg16; \
		op2.type = NONE; \
		break;

// OP inm8
#define dis_inm8(opcode, name) \
	case opcode: {\
		int inm = int(memory.read(PC++, GBMemory::DONT_WATCH)); \
		result << name << " 0x" << std::setw(2) << inm; \
		opcode_str = name; \
		op1_str  = ToString(inm); \
		op2_str  = ""; \
		op1_type = INM8; \
		op1.val  = inm; \
		op2.type = NONE; \
		break; \
	}

// OP inm16
#define dis_inm16(opcode, name) \
	case opcode: {\
		int inm = int(memory.read16(PC, GBMemory::DONT_WATCH)); \
        PC += 2; \
		result << name << " 0x" << std::setw(4) << inm; \
		opcode_str = name; \
		op1_str  = ToString(inm); \
		op2_str  = ""; \
		op1_type = INM16; \
		op1.val  = inm; \
		op2.type = NONE; \
		break; \
	}

#define dis_reg_inm(opcode, name, reg) \
	case opcode: {\
		int inm = int(memory.read(PC++, GBMemory::DONT_WATCH)); \
		result << name << " " << #reg << ", 0x" << std::setw(2) << inm; \
		opcode_str = name; \
		op1_str  = #reg; \
		op2_str  = ToString(inm); \
		op1_type = REG; \
		op1.reg  = reg; \
		op2.type = INM8; \
		op2.val  = inm; \
		break; \
	}

#define dis_reg16_inm(opcode, name, reg16) \
	case opcode: {\
		int inm = int(memory.read16(PC, GBMemory::DONT_WATCH)); \
		PC += 2; \
		result << name << " " << #reg16 << ", 0x" << std::setw(4) << inm; \
		opcode_str = name; \
		op1_str  = #reg16; \
		op2_str  = ToString(inm); \
		op1_type = REG; \
		op1.reg  = reg16; \
		op2.type = INM8; \
		op2.val  = inm; \
		break; \
	}

#define dis_reg16_inm8(opcode, name, reg16) \
	case opcode: {\
		int inm = int(memory.read(PC++, GBMemory::DONT_WATCH)); \
		result << name << " " << #reg16 << ", 0x" << std::setw(2) << inm; \
		opcode_str = name; \
		op1_str  = #reg; \
		op2_str  = ToString(inm); \
		op1_type = REG; \
		op1.reg  = reg; \
		op2.type = INM8; \
		op2.val  = inm; \
		break; \
	}

#define dis_reg_reg(opcode, name, reg1, reg2) \
	case opcode: \
		result << name << " " << #reg1 << ", " << #reg2; \
		opcode_str = name; \
		op1_str  = #reg1; \
		op2_str  = #reg2; \
		op1_type = REG; \
		op1.reg  = reg1; \
		op2.type = REG; \
		op2.val  = reg2; \
		break;

#define dis_A_reg(opcode, name, reg2) dis_reg_reg(opcode, name, A, reg2)
#define dis_B_reg(opcode, name, reg2) dis_reg_reg(opcode, name, B, reg2)
#define dis_C_reg(opcode, name, reg2) dis_reg_reg(opcode, name, C, reg2)
#define dis_D_reg(opcode, name, reg2) dis_reg_reg(opcode, name, D, reg2)
#define dis_E_reg(opcode, name, reg2) dis_reg_reg(opcode, name, E, reg2)
#define dis_H_reg(opcode, name, reg2) dis_reg_reg(opcode, name, H, reg2)
#define dis_L_reg(opcode, name, reg2) dis_reg_reg(opcode, name, L, reg2)

#define dis_reg16_reg16(opcode, name, reg16_1, reg16_2) \
	case opcode: \
		result << name << " " << #reg16_1 << ", " << #reg16_2; \
		opcode_str = name; \
		op1_str  = #reg16_1; \
		op2_str  = #reg16_2; \
		op1_type = REG; \
		op1.reg  = reg16_1; \
		op2.type = REG; \
		op2.val  = reg16_2; \
		break;

#define dis_HL_reg16(opcode, name, reg16_2) dis_reg16_reg16(opcode, name, HL, reg16_2)

// OP reg, (reg16)
#define dis_reg__reg16_(opcode, name, reg, reg16) \
	case opcode: \
		result << name << " " << #reg << ", (" << #reg16 << ")"; \
		opcode_str = name; \
		op1_str  = #reg; \
		op2_str  = std::string("(")+ #reg16 + ")"; \
		op1_type = REG; \
		op1.reg  = reg; \
		op2.type = MEM_INDIRECT; \
		op2.val  = reg16; \
		break;

// OP reg, (HL)
#define dis_reg__HL_(opcode, name, reg) dis_reg__reg16_(opcode, name, reg, HL)

// OP reg, (inm)
#define dis_reg__inm_(opcode, name, reg) \
	case opcode: {\
		int inm = int(memory.read16(PC, GBMemory::DONT_WATCH)); \
        PC += 2; \
		result << name << " " << #reg << ", (0x" << \
				std::setw(4) << inm << ")"; \
		opcode_str = name; \
		op1_str  = #reg; \
		op2_str  = std::string("(") + ToString(inm) + ")"; \
		op1_type = REG; \
		op1.reg  = reg; \
		op2.type = MEM_DIRECT; \
		op2.val  = inm; \
		break; \
	}

// OP (reg16), reg
#define dis__reg16__reg(opcode, name, reg16, reg) \
	case opcode: \
		result << name << " (" << #reg16 << "), " << #reg; \
		opcode_str = name; \
		op1_str  = std::string("(")+ #reg16 + ")"; \
		op2_str  = #reg; \
		op1.type = MEM_INDIRECT; \
		op1.val  = reg16; \
		op2_type = REG; \
		op2.reg  = reg; \
		break;

// OP (HL), reg
#define dis__HL__reg(opcode, name, reg) dis__reg16__reg(opcode, name, HL, reg)

// OP (inm), reg
#define dis__inm__reg(opcode, name, reg) \
	case opcode: {\
		int inm = int(memory.read16(PC, GBMemory::DONT_WATCH)); \
        PC += 2; \
		result << name << " (0x" << \
				std::setw(4) << inm << "), " << #reg; \
		opcode_str = name; \
		op1_str  = std::string("(") + ToString(inm) + ")"; \
		op2_str  = #reg; \
		op1.type = MEM_DIRECT; \
		op1.val  = inm; \
		op2_type = REG; \
		op2.reg  = reg; \
		break; \
	}

// OP (inm), reg16
#define dis__inm__reg16(opcode, name, reg16) \
	case opcode: {\
		int inm = int(memory.read16(PC, GBMemory::DONT_WATCH)); \
        PC += 2; \
		result << name << " (0x" << \
				std::setw(4) << inm << "), " << #reg16; \
		opcode_str = name; \
		op1_str  = std::string("(") + ToString(inm) + ")"; \
		op2_str  = #reg; \
		op1.type = MEM_DIRECT; \
		op1.val  = inm; \
		op2_type = REG; \
		op2.reg  = reg16; \
		break; \
	}

// OP (reg16), inm
#define dis__reg16__inm(opcode, name, reg16) \
	case opcode: {\
		int inm = int(memory.read(PC++, GBMemory::DONT_WATCH)); \
		result << name << " (" << #reg16 << "), 0x" << \
				std::setw(2) << inm; \
		opcode_str = name; \
		op1_str  = std::string("(") + #reg16 + ")"; \
		op2_str  = ToString(inm); \
		op1.type = MEM_INDIRECT; \
		op1.reg  = reg16; \
		op2.type = INM8; \
		op2.val  = inm; \
		break; \
	}


// Special routine for JR
#define dis_JR(opcode, name) \
	case opcode: { \
		s8 offset = memory.read(PC++); \
		result << name << " " << std::dec << int(offset) << "\t[0x" \
				 << std::hex << std::setw(2) << int(PC+offset) << "]"; \
		opcode_str = name; \
		op1_str  = ToString(int(offset)); \
		op2_str  = ToString(int(PC+offset)); \
		op1.type = INM8; \
		op1.reg  = offset; \
		op2.type = NONE; \
		op2.val  = int(PC+offset); \
		break; \
	}


////////////////////////////////////////////////////////////


#endif








