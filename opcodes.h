#define for_each_register(opA, opB, opC, opD, opE, opH, opL, macro) \
	macro(opA, A) \
	macro(opB, B) \
        macro(opC, C) \
        macro(opD, D) \
	macro(opE, E) \
	macro(opH, H) \
	macro(opL, L)

#define LD_reg_nn(opcode, reg) \
	case opcode: \
		regs.reg = memory[regs.PC++]; \
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
		regs.reg = memory[regs.HL]; \
		break;

// LD (HL), reg
#define LD__HL__reg(opcode, reg) \
	case opcode: \
		memory[regs.HL] = regs.reg; \
		break;

#define PUSH(opcode, regH, regL) \
	case opcode: \
		memory[regs.SP-1] = regs.regH; \
		memory[regs.SP-2] = regs.regL; \
		regs.SP -= 2; \
		break;

#define POP(opcode, regH, regL) \
	case opcode: \
		regs.regL = memory[regs.SP]; \
		regs.regH = memory[regs.SP+1]; \
		regs.SP += 2; \
		break;

#define ADD_A_reg(opcode, reg) \
	case opcode: { \
		int res = regs.A + regs.reg; \
		int half_res = (regs.A & 0x0F) + (regs.reg & 0x0F); \
		regs.A = static_cast<u8>(res); \
		\
		reset_flag(ADD_SUB_FLAG); \
		if (res > 0xFF)      set_flag(CARRY_FLAG); \
		if (regs.A == 0)     set_flag(ZERO_FLAG);  \
		if (half_res > 0x0F) set_flag(HALF_CARRY_FLAG); \
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
		if (res > 0xFF)      set_flag(CARRY_FLAG); \
		if (regs.A == 0)     set_flag(ZERO_FLAG);  \
		if (half_res > 0x0F) set_flag(HALF_CARRY_FLAG); \
		break; \
	}


