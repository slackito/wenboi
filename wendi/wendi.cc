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

// wendi, the WENboi DIsassembler :)

#include "../gbcore.h"
#include "../Logger.h"
#include "disassembly_output.h"
#include "output_txt.h"
#include "output_graph.h"
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
list<CodeBlock> ignore;


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
			block_name << "loc";
		}

		block_name << "_" << std::hex << std::setw(4) << std::setfill('0') << b.start;
	}

	b.name = block_name.str();
}

bool does_fall_through(const Instruction &ins)
{
	if (ins.type == Instruction::UNCONDITIONAL_RET ||
			ins.type == Instruction::UNCONDITIONAL_JUMP)
		return false;
	return true;
}


// true if it's a jump with known destination address
bool is_jump(const Instruction &ins)
{
	if (ins.type == Instruction::UNCONDITIONAL_JUMP ||
			ins.type == Instruction::CONDITIONAL_JUMP ||
			ins.type == Instruction::CALL ||
			ins.type == Instruction::RESET)
		return true;
	return false;
}

bool is_block_end(const Instruction &ins)
{
	if (is_jump(ins) ||
			ins.type == Instruction::CONDITIONAL_RET || 
			ins.type == Instruction::UNCONDITIONAL_RET)
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
				

void new_block_start(address dst, address src, Instruction::InstructionType type, CodeBlock &current, 
		             list<CodeBlock> &blocks, list<CodeBlock> &pending)
{
	if (dst == current.start) // Check if dst is this block's beginning
	{
		current.add_xref(src, type);
	}
	else if (dst > current.start && dst < current.end) // Check if dst is inside this block
	{
		logger.info("Splitting current block 0x", std::hex, current.start, " at 0x", dst);
		CodeBlock newblock(current, dst);
		blocks.push_back(current);
		current = newblock;
	}
	else
	{
		// Check if dst is inside a known block
		list<CodeBlock>::iterator i = find_block(blocks, dst);
		if (i != blocks.end())
		{
			if (dst == i->start)
			{
				i->add_xref(src, type);
			}
			else
			{
				logger.info("Splitting block 0x", std::hex, i->start, " at 0x", dst);
				blocks.push_back(CodeBlock(*i, dst));
				blocks.back().add_xref(src, type);
			}

		}
		else
		{
			// Check if dst is a pending block
			i = find_block(pending, dst);
			if (i != pending.end())
			{
				logger.info("Adding xref to pending block 0x", std::hex, i->start);
				i->add_xref(src, type);
			}
			else
			{
				// dst is a new block
				pending.push_back(CodeBlock(dst));
				pending.back().add_xref(src, type);
				logger.info("new block at ", std::hex, "0x", dst);
			}
		}
	}
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
			else if (cmd == "ignore")
			{
				address a;
				config >> std::hex >> a;
				ignore.push_back(CodeBlock(a));
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
		if (find_block(ignore, addr) != ignore.end())
		{
			logger.info("Ignoring block at 0x", std::hex, addr);
			continue;
		}
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
				new_block_start(dst, addr, ins.type, block, blocks, pending);
			}

			address new_addr = addr+ins.length;

			// If new addr is in another block, this block is over
			if (find_block(blocks, new_addr) != blocks.end() ||
				find_block(pending, new_addr) != pending.end() ||
				is_block_end(ins))
			{
				block_end=true;
				if (does_fall_through(ins))
				{
					new_block_start(new_addr, addr, Instruction::OTHER,
							block, blocks, pending);
				}
			}

			addr = new_addr;
		}

		blocks.push_back(block);
	}

	vector<CodeBlock> tmp;
	for (list<CodeBlock>::iterator i = blocks.begin(); i != blocks.end(); i++)
		tmp.push_back(*i);
	std::sort(tmp.begin(), tmp.end());	
	std::for_each(tmp.begin(), tmp.end(), classify_block);

	std::ofstream graph_stream((std::string(argv[1])+".dot").c_str());
	std::ofstream txt_stream((std::string(argv[1])+".txt").c_str());

	GraphDisassemblyOutput graph(graph_stream);
	TextDisassemblyOutput text(txt_stream);
	graph.generate_output(gb, tmp);
	text.generate_output(gb, tmp);

	return 0;
}
