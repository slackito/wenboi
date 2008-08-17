#include "../gbcore.h"
#include "../Logger.h"
#include "CodeBlock.h"
#include "disasm.h"

#include <vector>
#include <utility>
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cstdlib>
//#include <tr1/unordered_map>

using std::vector;
using std::list;
using std::pair;
using std::string;
//using std::tr1::unordered_map;

typedef u16 address;

list<CodeBlock> blocks;
list<CodeBlock> pending;


void classify_block(CodeBlock &b)
{
	std::ostringstream block_name;

	if (b.start == 0x100)
		b.type |= CodeBlock::ENTRYPOINT;

	// check for CALL xrefs; build xrefs list string
	std::sort(b.xrefs.begin(), b.xrefs.end());
	for(CodeBlock::XrefsConstIterator i=b.xrefs.begin();
			i!=b.xrefs.end();
			i++)
	{
		if (i->second == Instruction::CALL)
			b.type |= CodeBlock::FUNCTION;
		if (i->second == Instruction::JUMP_TABLE_JUMP)
			b.type |= CodeBlock::JUMP_TABLE_DEST;
		if (i->first == 0x40)
			b.type |= CodeBlock::VBLANK_HANDLER;
		if (i->first == 0x48)
			b.type |= CodeBlock::LCD_STAT_HANDLER;
		if (i->first == 0x50)
			b.type |= CodeBlock::TIMER_HANDLER;
		if (i->first == 0x58)
			b.type |= CodeBlock::SERIAL_HANDLER;
		if (i->first == 0x60)
			b.type |= CodeBlock::JOYPAD_HANDLER;
	}

	if (b.type & CodeBlock::ENTRYPOINT)
		block_name << "entrypoint";
	else if (b.start == 0x40)
		block_name << "vblank_interrupt";
	else if (b.start == 0x48)
		block_name << "lcd_stat_interrupt";
	else if (b.start == 0x50)
		block_name << "timer_interrupt";
	else if (b.start == 0x58)
		block_name << "serial_interrupt";
	else if (b.start == 0x60)
		block_name << "joypad_interrupt";
	else
	{
		if (b.type & 
				(CodeBlock::VBLANK_HANDLER |
				 CodeBlock::LCD_STAT_HANDLER |
				 CodeBlock::TIMER_HANDLER |
				 CodeBlock::SERIAL_HANDLER | 
				 CodeBlock::JOYPAD_HANDLER))
		{
			block_name << "handler";
		}
		else if (b.type & CodeBlock::FUNCTION)
		{
			block_name << "function";
		}
		else if (b.type & CodeBlock::JUMP_TABLE_DEST)
		{
			block_name << "jump";
		}
		else
		{
			block_name << "block";
		}

		block_name << "_0x" << std::hex << std::setw(4) << std::setfill('0') << b.start;
	}

	b.name = block_name.str();
}


void show_jump_table_block(GameBoy &gb, const CodeBlock &b)
{
	int n = (b.end - b.start)/2;

	std::cout << ";-- JUMP TABLE ------------------------" << std::endl;
	for (int i=0; i<n; i++)
	{
		address addr = b.start+2*i;
		std::cout << "\t0x" << std::hex << std::setw(4) <<  std::setfill('0') << addr <<
			"\t" <<  i << "\t0x" << std::setw(2) << gb.memory.read16(addr) << std::endl;;
	}
}



void show_disassembly_block(const CodeBlock& b)
{
	std::ostringstream xrefsstr;
	
	if (!b.xrefs.empty())
		xrefsstr << "\t;xrefs ";

	// check for CALL xrefs; build xrefs list string
	for(CodeBlock::XrefsConstIterator i=b.xrefs.begin();
			i!=b.xrefs.end();
			i++)
	{
		xrefsstr << std::hex << "0x" << std::setw(4) << std::setfill('0') << i->first << ",";
	}
	
	xrefsstr << std::endl;
	
	// Print header to stdout
	if (b.type & CodeBlock::FUNCTION)
		std::cout << std::endl << ";-- FUNCTION --------------------------" << std::endl;
	if (b.type & CodeBlock::VBLANK_HANDLER)
		std::cout << std::endl << ";-- VBLANK INTERRUPT HANDLER ----------" << std::endl;
	if (b.type & CodeBlock::LCD_STAT_HANDLER)
		std::cout << std::endl << ";-- LCD_STAT INTERRUPT HANDLER --------" << std::endl;
	if (b.type & CodeBlock::TIMER_HANDLER)
		std::cout << std::endl << ";-- TIMER INTERRUPT HANDLER -----------" << std::endl;
	if (b.type & CodeBlock::SERIAL_HANDLER)
		std::cout << std::endl << ";-- SERIAL INTERRUPT HANDLER ----------" << std::endl;
	if (b.type & CodeBlock::JOYPAD_HANDLER)
		std::cout << std::endl << ";-- JOYPAD INTERRUPT HANDLER ----------" << std::endl;

	if (b.type & CodeBlock::ENTRYPOINT)
		std::cout << std::endl << ";-- ENTRYPOINT ------------------------" << std::endl;
	
	std::cout << b.name << ":";

	std::cout << xrefsstr.str();

	// Print disassembly
	for(CodeBlock::DisassemblyConstIterator i=b.disassembly.begin();
			i!=b.disassembly.end();
			i++)
	{
		std::cout << std::hex << "\t0x" << 
			std::setw(4) << std::setfill('0') << i->first << 
			"\t" << i->second << std::endl;
	}
}

bool is_block_end(const Instruction &ins)
{
	if (ins.type == Instruction::UNCONDITIONAL_JUMP || 
			ins.type == Instruction::UNCONDITIONAL_RET ||
			ins.type == Instruction::RESET)
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
		else if (ins.op1.type == Instruction::INM16)
			return ins.op1.val;
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

void hexdump(GameBoy &gb, address start, address end)
{
	if (end > start)
	{
		std::cout << std::endl << ";-- HEXDUMP ---------------------------";
		address i=start - (start%0x10);
		if (i<start)
		{
			std::cout << std::endl << ";" << std::hex << "0x" << 
				std::setw(4) << std::setfill('0') << start << " ";
			while(i < start)
			{
				if (i % 0x10 == 8)
					std::cout << "- ";
				std::cout << "   ";
				i++;
			}
		}

		while(i<end)
		{
			if (i % 0x10 == 0)
				std::cout << std::endl << ";" << std::hex << "0x" << i << " ";

			if (i % 0x10 == 8)
				std::cout << "- ";

			std::cout << std::hex << std::setw(2) << std::setfill('0') << int(gb.memory.read(i)) << " ";
			i++;
		}

		std::cout << std::endl << std::endl;
	}
}

void show_disassembly(GameBoy &gb, vector<CodeBlock> &v)
{
	//std::for_each(tmp.begin(), tmp.end(), show_block);
	
	const address MAX_ADDRESS = 0xFFFF;
	address last = 0;
	for (vector<CodeBlock>::iterator i=v.begin();
			i != v.end(); i++)
	{
		hexdump(gb, last, i->start);
		switch (i->type)
		{
			case CodeBlock::JUMP_TABLE:
				show_jump_table_block(gb, *i);
				break;
			default:
				show_disassembly_block(*i);
		}
		last = i->end;
	}

	hexdump(gb, v.back().end, MAX_ADDRESS);
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

	if (argc > 2)
	{
		std::ifstream config(argv[2]);
		while (!config.eof())
		{
			std::string cmd;
			config >> cmd;
			if (cmd == "block")
			{
				address a;
				config >> std::hex >> a;
				pending.push_back(CodeBlock(a));
			}
			else if (cmd == "jump_table")
			{
				address start, end;
				config >> std::hex >> start >> end;
				logger.trace("jump table block at 0x",std::hex, start);
				blocks.push_back(CodeBlock(CodeBlock::JUMP_TABLE, start, end));
				// add all destinations to pending
				int n = (end - start)/2;
				for (int i=0; i<n; i++)
				{
					address src = start+2*i;
					address dst = gb.memory.read16(src);
					list<CodeBlock>::iterator i = find_block(pending, dst);
					if (i == pending.end())
					{
						logger.trace("jump table dst block at 0x",std::hex, dst);
						pending.push_back(CodeBlock(dst));
						pending.back().add_xref(src, Instruction::JUMP_TABLE_JUMP);
					}
					else
					{
						i->add_xref(src, Instruction::JUMP_TABLE_JUMP);
					}
				}
			}
		}
	}

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

			addr += ins.length;

			// If new addr is in another block, this block is over
			if (find_block(blocks, addr) != blocks.end() ||
				find_block(pending, addr) != pending.end() ||
				is_block_end(ins))
			{
				block_end=true;
			}
				


		}

		blocks.push_back(block);
	}

	vector<CodeBlock> tmp;
	for (list<CodeBlock>::iterator i = blocks.begin(); i != blocks.end(); i++)
		tmp.push_back(*i);
	std::sort(tmp.begin(), tmp.end());	
	std::for_each(tmp.begin(), tmp.end(), classify_block);

	show_disassembly(gb, tmp);

	return 0;
}
