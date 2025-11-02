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
#ifndef OUTPUT_GRAPH
#define OUTPUT_GRAPH
#include "CodeBlock.h"
#include "disassembly_output.h"
#include <iostream>
#include <vector>

class GraphDisassemblyOutput : public DisassemblyOutput {
  std::ostream &out;

  void show_disassembly_block(const CodeBlock &b);
  void show_jump_table_block(GameBoy &gb, const CodeBlock &b);
  void show_xrefs(std::vector<CodeBlock> &v);

public:
  GraphDisassemblyOutput(std::ostream &ofs) : out(ofs) {}
  void generate_output(GameBoy &gb, std::vector<CodeBlock> &v);
};

#endif
