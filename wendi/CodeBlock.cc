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
#include "CodeBlock.h"

#include <algorithm>
#include <ext/functional>

CodeBlock::CodeBlock(address start): //< creates an empty CodeBlock 
	type(BLOCK),
	start(start),
	end(start),
	disassembly(),
	xrefs(),
	name()
{}

CodeBlock::CodeBlock(CodeBlock &block, address addr): //< removes [addr,end[ from the block creating a new one
	type(BLOCK),
	start(addr),
	end(block.end),
	disassembly(),
	xrefs(),
	name()
{
	using std::bind2nd;
	using __gnu_cxx::select1st;
	using std::equal_to;
	block.end = addr;

	// HAHA! I'M USING STL EXTENSIONS!!!1
	DisassemblyIterator first = std::find_if(block.disassembly.begin(),
			block.disassembly.end(),
			compose1(bind2nd(equal_to<address>(), addr),
				select1st<DisassemblyItem>()));
	DisassemblyIterator tmp = first;
	this->add_xref((--tmp)->first, Instruction::OTHER);
	DisassemblyIterator last  = block.disassembly.end();
	
	disassembly.splice(disassembly.end(), block.disassembly, first, last);
}

CodeBlock::CodeBlock(CodeBlockType type, address start, address end):
	type(type),
	start(start),
	end(end),
	disassembly(),
	xrefs(),
	name()
{
}

void CodeBlock::add_instruction(std::string ins, int nbytes) // appends an instruction to the end of the block
{
	disassembly.push_back(std::make_pair(end,ins));
	end += nbytes;
}

void CodeBlock::add_xref(address addr, Instruction::InstructionType jt)
{
	xrefs.push_back(std::make_pair(addr, jt));
}



