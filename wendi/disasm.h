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
#ifndef DISASM_H
#define DISASM_H

#include "../gbcore.h"
#include "Instruction.h"

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
