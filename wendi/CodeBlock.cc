#include "CodeBlock.h"

#include <algorithm>
#include <ext/functional>

CodeBlock::CodeBlock(address start): //< creates an empty CodeBlock 
	start(start),
	end(start),
	disassembly(),
	xrefs()
{}

CodeBlock::CodeBlock(CodeBlock &block, address addr): //< removes [addr,end[ from the block creating a new one
	start(addr),
	end(block.end),
	disassembly(),
	xrefs()
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
	DisassemblyIterator last  = block.disassembly.end();
	disassembly.splice(disassembly.end(), block.disassembly, first, last);
}


void CodeBlock::add_instruction(std::string ins, int nbytes) // appends an instruction to the end of the block
{
	disassembly.push_back(std::make_pair(end,ins));
	end += nbytes;
}

void CodeBlock::add_xref(address addr, JumpType jt)
{
	xrefs.push_back(std::make_pair(addr, jt));
}



