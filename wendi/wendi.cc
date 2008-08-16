#include "../gbcore.h"
#include "../Logger.h"
#include "CodeBlock.h"
#include "disasm.h"

#include <vector>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
//#include <tr1/unordered_map>

using std::vector;
using std::list;
using std::pair;
using std::string;
//using std::tr1::unordered_map;

typedef u16 address;

list<CodeBlock> blocks;
list<CodeBlock> pending;

void show_block(const CodeBlock& b)
{
	bool is_CALLed = false;
	bool is_VBLANK_interrupt_handler   = false;
	bool is_LCD_STAT_interrupt_handler = false;
	bool is_TIMER_interrupt_handler    = false;
	bool is_SERIAL_interrupt_handler   = false;
	bool is_JOYPAD_interrupt_handler   = false;
	std::ostringstream headerstr;
	
	// Create header, check for CALL xrefs
	headerstr << "block_0x" << std::hex << b.start << ":";
	if (!b.xrefs.empty())
		headerstr << "\t;xrefs ";

	for(CodeBlock::XrefsConstIterator i=b.xrefs.begin();
			i!=b.xrefs.end();
			i++)
	{
		headerstr << std::hex << "0x" << i->first << ",";
		if (i->second == Instruction::CALL)
			is_CALLed = true;
		if (i->first == 0x40)
			is_VBLANK_interrupt_handler   = true;
		if (i->first == 0x48)
			is_LCD_STAT_interrupt_handler = true;
		if (i->first == 0x50)
			is_TIMER_interrupt_handler    = true;
		if (i->first == 0x58)
			is_SERIAL_interrupt_handler   = true;
		if (i->first == 0x60)
			is_JOYPAD_interrupt_handler   = true;
	}
	
	headerstr << std::endl;
	
	// Print header to stdout
	if (is_CALLed)
		std::cout << std::endl << "-- FUNCTION --------------------------" << std::endl;
	if (is_VBLANK_interrupt_handler)
		std::cout << std::endl << "-- VBLANK INTERRUPT HANDLER ----------" << std::endl;
	if (is_LCD_STAT_interrupt_handler)
		std::cout << std::endl << "-- LCD_STAT INTERRUPT HANDLER --------" << std::endl;
	if (is_TIMER_interrupt_handler)
		std::cout << std::endl << "-- TIMER INTERRUPT HANDLER -----------" << std::endl;
	if (is_SERIAL_interrupt_handler)
		std::cout << std::endl << "-- SERIAL INTERRUPT HANDLER ----------" << std::endl;
	if (is_JOYPAD_interrupt_handler)
		std::cout << std::endl << "-- JOYPAD INTERRUPT HANDLER ----------" << std::endl;

	if (b.start == 0x100)
		std::cout << std::endl << "-- ENTRYPOINT -----------------" << std::endl;

	std::cout << headerstr.str();

	// Print disassembly
	for(CodeBlock::DisassemblyConstIterator i=b.disassembly.begin();
			i!=b.disassembly.end();
			i++)
		std::cout << std::hex << "0x" << i->first << "\t" << i->second << std::endl;
}

bool is_block_end(const Instruction &ins)
{
	if (ins.type == Instruction::UNCONDITIONAL_JUMP || 
			ins.type == Instruction::RET)
		return true;

	return false;
}

bool is_jump(const Instruction &ins)
{
	if (ins.type == Instruction::UNCONDITIONAL_JUMP ||
			ins.type == Instruction::CONDITIONAL_JUMP ||
			ins.type == Instruction::CALL ||
			ins.type == Instruction::RESET)
		return true;
	return false;
}

address jump_address(const Instruction &ins)
{
	if (ins.type == Instruction::UNCONDITIONAL_JUMP ||
			ins.type == Instruction::CONDITIONAL_JUMP ||
			ins.type == Instruction::CALL)
	{
		if (ins.str.substr(0,2)=="JR")
			return ins.op2.val;
		else return ins.op1.val;
	}
	else // RESET
	{
		switch(ins.opcode)
		{
			case 0xC7: return 0x00;
			case 0xCF: return 0x08;
			case 0xD7: return 0x10;
			case 0xDF: return 0x18;
			case 0xE7: return 0x20;
			case 0xEF: return 0x28;
			case 0xF7: return 0x30;
			case 0xFF: return 0x38;
		}
	}

	return 0;
}

list<CodeBlock>::iterator find_block(list<CodeBlock> &l, address addr)
{
	//logger.trace("--> find_block()");
	list<CodeBlock>::iterator i = l.begin();
	while(i != l.end())
	{
		//logger.trace("finding block. addr=0x", std::hex, addr, " start=0x", i->start, " end=0x", i->end);
		if ((addr >= i->start && addr < i->end) ||
				(addr == i->start && addr == i->end))
		{
			//logger.trace("FOUND!");
			break;
		}
		i++;
	}
	//logger.trace("i==l.end()? --> ", i == l.end());
	//logger.trace("<-- find_block()");
	return i;
}

int main(int argc, char **argv)
{
	logger.set_log_level(Logger::TRACE);
	GameBoy gb(argv[1]);

	list<CodeBlock> pending;
	// pending holds empty CodeBlocks
	pending.push_back(CodeBlock(0x100)); // entrypoint
	pending.push_back(CodeBlock(0x40));  // interrupt handlers
	pending.push_back(CodeBlock(0x48));
	pending.push_back(CodeBlock(0x50));
	pending.push_back(CodeBlock(0x58));
	pending.push_back(CodeBlock(0x60));

	while(!pending.empty())
	{
		// Disassemble a block
		CodeBlock block = pending.front();
		pending.pop_front();

		address addr = block.start;
		logger.info("Starting disassembly of block 0x", std::hex, addr);

		bool block_end = false;
		while(!block_end)
		{
			Instruction ins(disassemble_opcode(gb, addr));
			block.add_instruction(ins.all, ins.length);

			if (is_jump(ins))
			{
				address dst = jump_address(ins);
				logger.info("Found jump. dst address = 0x", std::hex, dst);
				if (dst == block.start) // Check if dst is this block's beginning
				{
					block.add_xref(addr, ins.type);
				}
				else if (dst > block.start && dst < block.end) // Check if dst is inside this block
				{
					logger.info("Splitting current block 0x", std::hex, block.start, " at 0x", dst);
					CodeBlock newblock(block, dst);
					blocks.push_back(block);
					block = newblock;
				}
				else
				{
					// Check if dst is inside a known block
					list<CodeBlock>::iterator i = find_block(blocks, dst);
					if (i != blocks.end())
					{
						if (dst == i->start)
						{
							i->add_xref(addr, ins.type);
						}
						else
						{
							logger.info("Splitting block 0x", std::hex, i->start, " at 0x", dst);
							blocks.push_back(CodeBlock(*i, dst));
							blocks.back().add_xref(addr, ins.type);
						}
						
					}
					else
					{
						// Check if dst is a pending block
						i = find_block(pending, dst);
						if (i != pending.end())
						{
							logger.info("Adding xref to pending block 0x", std::hex, i->start);
							i->add_xref(addr, ins.type);
						}
						else
						{
							// dst is a new block
							pending.push_back(CodeBlock(dst));
							pending.back().add_xref(addr, ins.type);
							logger.info("new block at ", std::hex, "0x", dst);
						}
					}
				}
			}

			if (is_block_end(ins))
			{
				block_end=true;
			}

			addr += ins.length;
		}

		blocks.push_back(block);
	}

	vector<CodeBlock> tmp;
	for (list<CodeBlock>::iterator i = blocks.begin(); i != blocks.end(); i++)
		tmp.push_back(*i);
	std::sort(tmp.begin(), tmp.end());	
	std::for_each(tmp.begin(), tmp.end(), show_block);
	
	return 0;
}
