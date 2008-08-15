#include "../gbcore.h"
#include "CodeBlock.h"

#include <vector>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <tr1/unordered_map>

using std::vector;
using std::list;
using std::pair;
using std::string;
using std::tr1::unordered_map;

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
			string ins;
			int len;
			gb.disassemble_opcode(addr, ins, len);
			blocks.back().add_instruction(ins, len);
			addr += len;
		}
	}
}
