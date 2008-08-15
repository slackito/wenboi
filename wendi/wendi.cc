#include "../gbcore.h"
#include "CodeBlock.h"
#include "disasm.h"

#include <vector>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <algorithm>
//#include <tr1/unordered_map>

using std::vector;
using std::list;
using std::pair;
using std::string;
//using std::tr1::unordered_map;

typedef u16 address;

list<CodeBlock> blocks;

void show_block(const CodeBlock& b)
{
	std::cout << "block_0x" << std::hex << b.start << ":" << std::endl;
	for(CodeBlock::DisassemblyConstIterator i=b.disassembly.begin();
			i!=b.disassembly.end();
			i++)
		std::cout << std::hex << "0x" << i->first << "\t" << i->second << std::endl;
}


int main(int argc, char **argv)
{
	GameBoy gb(argv[1]);

	vector<address> pending;
	pending.push_back(0x100);

	while(!pending.empty())
	{
		// Disassemble a block
		int addr = pending.back();
		pending.pop_back();

		blocks.push_back(CodeBlock(addr));

		bool jump_reached = false;
		while(!jump_reached)
		{
			Instruction ins(disassemble_opcode(gb, addr));
			blocks.back().add_instruction(ins.all, ins.length);
			addr += ins.length;

			if (ins.is_jump()) jump_reached=true;
		}
	}

	//std::for_each(blocks, show_block);
	for (list<CodeBlock>::iterator i = blocks.begin(); i != blocks.end(); i++)
		show_block(*i);
	return 0;
}
