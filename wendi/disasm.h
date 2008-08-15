#ifndef DISASM_H
#define DISASM_H

#include <string>
#include <sstream>

struct Instruction
{
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

	union Operand {
		Register reg;
		int      val;
	};

	int length;
	std::string all;

	std::string opcode_str, op1_str, op2_str;
	OperandType op1_type, op2_type;
	Operand     op1, op2;
};

template <class T> 
std::string ToString(const T &object)
{
	std::ostringstream os;
	os << object;
	return(os.str());
}

Instruction disassemble_opcode(GameBoy &gb, u16 addr);
std::string get_port_name(int port);

#endif
