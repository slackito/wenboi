#ifndef INSTRUCTION_H
#define INSTRUCTION_H

struct Instruction
{
	enum InstructionType
	{
		JUMP,
		CALL,
		RESET,
		ALU,
		LOAD,

	};

	enum Register { A=0,B,C,D,E,H,L,AF,BC,DE,HL,SP,PC };

	enum OperandType
	{
		NONE=0,
		REG,
		MEM_DIRECT,
		MEM_INDIRECT,
		INM8,
		INM16
	};

	struct Operand
	{
		std::string str;
		OperandType type;
		// FIXME: An anonymous union doesn't work here :(
		// union {
		Register reg;
		int val;
		// }

		Operand(): str(""), type(NONE) {}
	};

	int length;
	std::string all;

	u8 opcode;
	u8 sub_opcode;
	std::string str;
	Operand     op1, op2;

	Instruction(int length, u8 opcode, u8 sub_opcode, std::string all, std::string opcode_str, Operand op1, Operand op2):
		length(length), opcode(opcode), sub_opcode(sub_opcode),all(all), str(opcode_str), op1(op1), op2(op2)
	{
	}
};

#endif

