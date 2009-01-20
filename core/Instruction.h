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
#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "../common/sized_types.h"

struct Instruction
{
	enum InstructionType
	{
		UNCONDITIONAL_JUMP=0,
		CONDITIONAL_JUMP,
		UNCONDITIONAL_RET,
		CONDITIONAL_RET,
		CALL,
		RESET,
		ALU,
		LOAD,
		OTHER,
		JUMP_TABLE_JUMP,
	};

	enum InstructionSubType
	{
		JP,
		JR,
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

		Operand(): str(""), type(NONE), reg(A), val(-1) {}
	};

	int length;
	InstructionType type;
	std::string all;

	u8 opcode;
	u8 sub_opcode;
	std::string str;
	Operand     op1, op2;

	Instruction(int length, InstructionType type, u8 opcode, u8 sub_opcode,
			std::string all, std::string opcode_str, Operand op1, Operand op2):
		length(length), type(type), all(all), 
		opcode(opcode), sub_opcode(sub_opcode), str(opcode_str), op1(op1), op2(op2)
	{
	}
};

#endif

