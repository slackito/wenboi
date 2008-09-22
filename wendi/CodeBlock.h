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
#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include <vector>
#include <utility>
#include <string>
#include <list>
#include "Instruction.h"
#include "../sized_types.h"

typedef u16 address;

class CodeBlock
{
	public:
	enum CodeBlockType
	{
		BLOCK            = 0x000,
		FUNCTION         = 0x001,
		VBLANK_HANDLER   = 0x002,
		LCD_STAT_HANDLER = 0x004,
		TIMER_HANDLER    = 0x008,
		SERIAL_HANDLER   = 0x010,
		JOYPAD_HANDLER   = 0x020,
		ENTRYPOINT       = 0x040,
		JUMP_TABLE       = 0x080,
		JUMP_TABLE_DEST  = 0x100,
	};

	typedef std::pair<address, std::string> DisassemblyItem;
	typedef std::pair<address, Instruction::InstructionType> XrefsItem;
	typedef std::list<DisassemblyItem>      DisassemblyList;
	typedef std::vector<XrefsItem>          XrefsVector;
	
	typedef DisassemblyList::iterator       DisassemblyIterator;
	typedef DisassemblyList::const_iterator DisassemblyConstIterator;
	typedef XrefsVector::iterator           XrefsIterator;
	typedef XrefsVector::const_iterator     XrefsConstIterator;

	int type;

	address start, end;  // block is [start, end[
	DisassemblyList disassembly;
	XrefsVector xrefs;

	std::string name;

	CodeBlock(address start); //< creates an empty CodeBlock 
	CodeBlock(CodeBlock &block, address addr); //< removes [addr,end[ from the block creating a new one
	CodeBlock(CodeBlockType type, address start, address end); // Creates a "raw" block

	int length() { return end-start; }
	void add_instruction(std::string ins, int nbytes); // appends an instruction to the end of the block
	void add_xref(address addr, Instruction::InstructionType jt);

	bool operator< (const CodeBlock& other) const { return start < other.start; }
};



#endif

