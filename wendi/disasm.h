#ifndef DISASM_H
#define DISASM_H

#include "../gbcore.h"

#include <string>
#include <sstream>

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
