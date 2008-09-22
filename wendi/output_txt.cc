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
#include "output_txt.h"
#include <sstream>
#include <iomanip>

using std::vector;
using std::string;

void TextDisassemblyOutput::show_disassembly_block(const CodeBlock& b)
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
		out << std::endl << ";-- FUNCTION --------------------------" << std::endl;
	if (b.type & CodeBlock::VBLANK_HANDLER)
		out << std::endl << ";-- VBLANK INTERRUPT HANDLER ----------" << std::endl;
	if (b.type & CodeBlock::LCD_STAT_HANDLER)
		out << std::endl << ";-- LCD_STAT INTERRUPT HANDLER --------" << std::endl;
	if (b.type & CodeBlock::TIMER_HANDLER)
		out << std::endl << ";-- TIMER INTERRUPT HANDLER -----------" << std::endl;
	if (b.type & CodeBlock::SERIAL_HANDLER)
		out << std::endl << ";-- SERIAL INTERRUPT HANDLER ----------" << std::endl;
	if (b.type & CodeBlock::JOYPAD_HANDLER)
		out << std::endl << ";-- JOYPAD INTERRUPT HANDLER ----------" << std::endl;

	if (b.type & CodeBlock::ENTRYPOINT)
		out << std::endl << ";-- ENTRYPOINT ------------------------" << std::endl;
	
	out << b.name << ":";

	out << xrefsstr.str();

	// Print disassembly
	for(CodeBlock::DisassemblyConstIterator i=b.disassembly.begin();
			i!=b.disassembly.end();
			i++)
	{
		out << std::hex << "\t0x" << 
			std::setw(4) << std::setfill('0') << i->first << 
			"\t" << i->second << std::endl;
	}
}

void TextDisassemblyOutput::show_jump_table_block(GameBoy &gb, const CodeBlock &b)
{
	int n = (b.end - b.start)/2;

	out << ";-- JUMP TABLE ------------------------" << std::endl;
	for (int i=0; i<n; i++)
	{
		address addr = b.start+2*i;
		out << "\t0x" << std::hex << std::setw(4) <<  std::setfill('0') << addr <<
			"\t" <<  i << "\t0x" << std::setw(2) << gb.memory.read16(addr) << std::endl;;
	}
}

void TextDisassemblyOutput::hexdump(GameBoy &gb, address start, address end)
{
	if (end > start)
	{
		out << std::endl << ";-- HEXDUMP ---------------------------";
		address i=start - (start%0x10);
		if (i<start)
		{
			out << std::endl << ";" << std::hex << "0x" << 
				std::setw(4) << std::setfill('0') << start << " ";
			while(i < start)
			{
				if (i % 0x10 == 8)
					out << "- ";
				out << "   ";
				i++;
			}
		}

		while(i<end)
		{
			if (i % 0x10 == 0)
				out << std::endl << ";" << std::hex << "0x" << i << " ";

			if (i % 0x10 == 8)
				out << "- ";

			out << std::hex << std::setw(2) << std::setfill('0') << int(gb.memory.read(i)) << " ";
			i++;
		}

		out << std::endl << std::endl;
	}
}

void TextDisassemblyOutput::generate_output(GameBoy &gb, vector<CodeBlock> &v)
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

