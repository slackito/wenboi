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
#include "CodeBlock.h"

#include <algorithm>

CodeBlock::CodeBlock(address start)
    : //< creates an empty CodeBlock
      type(BLOCK), start(start), end(start), disassembly(), xrefs(), name() {}

CodeBlock::CodeBlock(CodeBlock &block, address addr)
    : //< removes [addr,end[ from the block creating a new one
      type(BLOCK), start(addr), end(block.end), disassembly(), xrefs(), name() {
  block.end = addr;

  DisassemblyIterator tmp;
  for (tmp = block.disassembly.begin(); tmp != block.disassembly.end(); ++tmp) {
    if (tmp->first == addr)
      break;
  }

  DisassemblyIterator first = tmp;

  this->add_xref((--tmp)->first, Instruction::OTHER);
  DisassemblyIterator last = block.disassembly.end();

  disassembly.splice(disassembly.end(), block.disassembly, first, last);
}

CodeBlock::CodeBlock(CodeBlockType type, address start, address end)
    : type(type), start(start), end(end), disassembly(), xrefs(), name() {}

void CodeBlock::add_instruction(
    std::string ins,
    int nbytes) // appends an instruction to the end of the block
{
  disassembly.push_back(std::make_pair(end, ins));
  end += nbytes;
}

void CodeBlock::add_xref(address addr, Instruction::InstructionType jt) {
  xrefs.push_back(std::make_pair(addr, jt));
}
