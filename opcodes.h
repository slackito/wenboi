#define set_flag_if(cond, flag) \
	if (cond) set_flag(flag); \
	else reset_flag(flag)

#define reset_flag_if(cond, flag) \
	if (cond) reset_flag(flag); \
	else set_flag(flag)

#define for_each_register(opA, opB, opC, opD, opE, opH, opL, macro) \
	macro(opA, A) \
	macro(opB, B) \
        macro(opC, C) \
        macro(opD, D) \
	macro(opE, E) \
	macro(opH, H) \
	macro(opL, L)

#define for_each_register16(opBC, opDE, opHL, opSP, macro) \
	macro(opBC, BC) \
	macro(opDE, DE) \
	macro(opHL, HL) \
	macro(opSP, SP) 

#define LD_reg_nn(opcode, reg) \
	case opcode: \
		regs.reg = memory.read(regs.PC++); \
		break;

#define LD_reg_reg(opcode, reg1, reg2) \
	case opcode: \
		regs.reg1 = regs.reg2; \
		break;

#define LD_A_reg(opcode, reg2) LD_reg_reg(opcode, A, reg2)
#define LD_B_reg(opcode, reg2) LD_reg_reg(opcode, B, reg2)
#define LD_C_reg(opcode, reg2) LD_reg_reg(opcode, C, reg2)
#define LD_D_reg(opcode, reg2) LD_reg_reg(opcode, D, reg2)
#define LD_E_reg(opcode, reg2) LD_reg_reg(opcode, E, reg2)
#define LD_H_reg(opcode, reg2) LD_reg_reg(opcode, H, reg2)
#define LD_L_reg(opcode, reg2) LD_reg_reg(opcode, L, reg2)

// LD reg, (HL)
#define LD_reg__HL_(opcode, reg) \
	case opcode: \
		regs.reg = memory.read(regs.HL); \
		break;

// LD (HL), reg
#define LD__HL__reg(opcode, reg) \
	case opcode: \
		memory.write(regs.HL, regs.reg); \
		break;

#define PUSH(opcode, regH, regL) \
	case opcode: \
		memory.write(regs.SP-1, regs.regH); \
		memory.write(regs.SP-2, regs.regL); \
		regs.SP -= 2; \
		break;

#define POP(opcode, regH, regL) \
	case opcode: \
		regs.regL = memory.read(regs.SP); \
		regs.regH = memory.read(regs.SP+1); \
		regs.SP += 2; \
		break;

#define ADD_A_reg(opcode, reg) \
	case opcode: { \
		int res = regs.A + regs.reg; \
		int half_res = (regs.A & 0x0F) + (regs.reg & 0x0F); \
		regs.A = static_cast<u8>(res); \
		\
		reset_flag(ADD_SUB_FLAG); \
		set_flag_if (res > 0xFF,      CARRY_FLAG); \
		set_flag_if (regs.A == 0,     ZERO_FLAG);  \
		set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG); \
		break; \
	}

#define ADC_A_reg(opcode, reg) \
	case opcode: { \
		int carry = (check_flag(CARRY_FLAG)? 1 : 0); \
		int res = regs.A + regs.reg + carry; \
		int half_res = (regs.A & 0x0F) + (regs.reg & 0x0F) + carry; \
		regs.A = static_cast<u8>(res); \
		\
		reset_flag(ADD_SUB_FLAG); \
		set_flag_if (res > 0xFF,      CARRY_FLAG); \
		set_flag_if (regs.A == 0,     ZERO_FLAG);  \
		set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG); \
		break; \
	}

#define SUB_reg(opcode, reg) \
	case opcode: { \
		int res = regs.A - regs.reg; \
		int half_res = (regs.A & 0x0F) - (regs.reg & 0x0F); \
		regs.A = static_cast<u8>(res); \
		set_flag(ADD_SUB_FLAG); \
		set_flag_if (res < 0,      CARRY_FLAG); \
		set_flag_if (res == 0,     ZERO_FLAG); \
		set_flag_if (half_res < 0, HALF_CARRY_FLAG); \
		break; \
	}

#define SBC_reg(opcode, reg) \
	case opcode: { \
		int carry = (check_flag(CARRY_FLAG)? 1 : 0); \
		int res = regs.A - regs.reg - carry; \
		int half_res = (regs.A & 0x0F) - (regs.reg & 0x0F) - carry; \
		regs.A = static_cast<u8>(res); \
		set_flag(ADD_SUB_FLAG); \
		set_flag_if (res < 0,      CARRY_FLAG); \
		set_flag_if (res == 0,     ZERO_FLAG); \
		set_flag_if (half_res < 0, HALF_CARRY_FLAG); \
		break; \
	}

#define AND_reg(opcode, reg) \
	case opcode: { \
		regs.A &= regs.reg; \
		if (regs.A == 0) set_flag(ZERO_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		set_flag(HALF_CARRY_FLAG); \
		reset_flag(CARRY_FLAG); \
		break; \
	}

#define OR_reg(opcode, reg) \
	case opcode: { \
		regs.A |= regs.reg; \
		if (regs.A == 0) set_flag(ZERO_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		reset_flag(HALF_CARRY_FLAG); \
		reset_flag(CARRY_FLAG); \
		break; \
	}

#define XOR_reg(opcode, reg) \
	case opcode: { \
		regs.A ^= regs.reg; \
		if (regs.A == 0) set_flag(ZERO_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		reset_flag(HALF_CARRY_FLAG); \
		reset_flag(CARRY_FLAG); \
		break; \
	}

#define CP_reg(opcode, reg) \
	case opcode: { \
		int res = regs.A - regs.reg; \
		int half_res = (regs.A & 0x0F) - (regs.reg & 0x0F); \
		set_flag(ADD_SUB_FLAG); \
		set_flag_if (res < 0,      CARRY_FLAG); \
		set_flag_if (res == 0,     ZERO_FLAG); \
		set_flag_if (half_res < 0, HALF_CARRY_FLAG); \
		break; \
	}

#define INC_reg(opcode, reg) \
	case opcode: {\
		int half_res = (regs.reg & 0x0F) + 1; \
		++regs.reg; \
		reset_flag(ADD_SUB_FLAG); \
		set_flag_if (regs.reg == 0,   ZERO_FLAG); \
		set_flag_if (half_res > 0x0F, HALF_CARRY_FLAG); \
		break; \
	}

#define DEC_reg(opcode, reg) \
	case opcode: {\
		int half_res = (regs.reg & 0x0F) - 1; \
		--regs.reg; \
		set_flag(ADD_SUB_FLAG); \
		set_flag_if (regs.reg == 0, ZERO_FLAG); \
		set_flag_if (half_res < 0, HALF_CARRY_FLAG); \
		break; \
	}

		
#define ADD_HL_reg16(opcode, reg16) \
	case opcode: {\
		int res = regs.HL + regs.reg16; \
		int half_res = (regs.HL & 0xFFF) + (regs.reg16 & 0xFFF); \
		regs.HL = static_cast<u16>(res); \
		reset_flag(ADD_SUB_FLAG); \
		set_flag_if (res == 0,         ZERO_FLAG); \
		set_flag_if (half_res > 0xFFF, HALF_CARRY_FLAG); \
		set_flag_if (res > 0xFFFF,     CARRY_FLAG); \
		break; \
	}

#define INC_reg16(opcode, reg16) \
	case opcode: \
		++regs.reg16; \
		break;

#define DEC_reg16(opcode, reg16) \
	case opcode: \
		--regs.reg16; \
		break;

#define SWAP_reg(opcode, reg) \
	case opcode: \
		regs.reg = ((regs.reg & 0x0F)<<4) | ((regs.reg & 0xF0)>> 4); \
		set_flag_if (regs.reg == 0, ZERO_FLAG); \
		reset_flag(CARRY_FLAG); \
		reset_flag(HALF_CARRY_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		break;

#define RST(opcode, n) \
	case opcode: \
		do_call(n); \
		break;


// TODO: Check which of GBCPUman.pdf or
// worldofspectrum z80 reference is correct
//set_flag_if(bit7, CARRY_FLAG);
//
#define RLC_reg(opcode, reg) \
	case opcode: { \
		u8 bit7 = regs.reg >> 7; \
		regs.reg = (regs.reg << 1) | bit7; \
		set_flag_if(regs.reg == 0, ZERO_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		reset_flag(HALF_CARRY_FLAG); \
		break; \
	}

#define RL_reg(opcode, reg) \
	case opcode: { \
		u8 bit7 = regs.reg >> 7; \
		regs.reg = (regs.reg << 1) | check_flag(CARRY_FLAG); \
		set_flag_if(bit7, CARRY_FLAG); \
		set_flag_if(regs.reg == 0, ZERO_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		reset_flag(HALF_CARRY_FLAG); \
		break; \
	}

// TODO: Check which of GBCPUman.pdf or
// worldofspectrum z80 reference is correct
//set_flag_if(bit7, CARRY_FLAG);
//
#define RRC_reg(opcode, reg) \
	case opcode: { \
		u8 bit0 = regs.reg & 1; \
		regs.reg = (regs.reg >> 1) | (bit0 << 7); \
		set_flag_if(regs.reg == 0, ZERO_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		reset_flag(HALF_CARRY_FLAG); \
		break; \
	}

#define RR_reg(opcode, reg) \
	case opcode: { \
		u8 bit0 = regs.reg & 1; \
		regs.reg = (regs.reg >> 1) | (check_flag(CARRY_FLAG) << 7); \
		set_flag_if(bit0, CARRY_FLAG); \
		set_flag_if(regs.reg == 0, ZERO_FLAG); \
		reset_flag(ADD_SUB_FLAG); \
		reset_flag(HALF_CARRY_FLAG); \
		break; \
	}










