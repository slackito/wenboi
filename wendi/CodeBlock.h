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
	typedef std::pair<address, std::string> DisassemblyItem;
	typedef std::pair<address, Instruction::InstructionType> XrefsItem;
	typedef std::list<DisassemblyItem>      DisassemblyList;
	typedef std::vector<XrefsItem>          XrefsVector;
	
	typedef DisassemblyList::iterator       DisassemblyIterator;
	typedef DisassemblyList::const_iterator DisassemblyConstIterator;
	typedef XrefsVector::iterator           XrefsIterator;
	typedef XrefsVector::const_iterator     XrefsConstIterator;

	address start, end;  // block is [start, end[
	DisassemblyList disassembly;
	XrefsVector xrefs;


	CodeBlock(address start); //< creates an empty CodeBlock 
	CodeBlock(CodeBlock &block, address addr); //< removes [addr,end[ from the block creating a new one

	int length() { return end-start; }
	void add_instruction(std::string ins, int nbytes); // appends an instruction to the end of the block
	void add_xref(address addr, Instruction::InstructionType jt);

	bool operator< (const CodeBlock& other) const { return start < other.start; }
};



#endif

