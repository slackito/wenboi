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



