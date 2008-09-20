#ifndef OUTPUT_GRAPH
#define OUTPUT_GRAPH
#include "disassembly_output.h"
#include "CodeBlock.h"
#include <vector>
#include <iostream>

class GraphDisassemblyOutput: public DisassemblyOutput
{
	std::ostream &out;

	void show_disassembly_block(const CodeBlock& b);
	void show_jump_table_block(GameBoy &gb, const CodeBlock &b);
	void show_xrefs(std::vector<CodeBlock> &v);

	public:
	GraphDisassemblyOutput(std::ostream &ofs): out(ofs) {}
	void generate_output(GameBoy &gb, std::vector<CodeBlock> &v);
};

#endif


