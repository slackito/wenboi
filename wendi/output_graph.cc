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
#include "output_graph.h"
#include "../common/Logger.h"
#include <iomanip>
#include <sstream>

using std::endl;
using std::string;
using std::vector;

void GraphDisassemblyOutput::show_disassembly_block(const CodeBlock &b) {
  std::ostringstream addrstr, inststr;

  // Generate an input port "<in>" for the node at the label
  out << "node [label=\"{{<in>";

  // Print header to stdout
  if (b.type & CodeBlock::FUNCTION)
    out << ";-- FUNCTION --------------------------\\l";
  if (b.type & CodeBlock::VBLANK_HANDLER)
    out << ";-- VBLANK INTERRUPT HANDLER ----------\\l";
  if (b.type & CodeBlock::LCD_STAT_HANDLER)
    out << ";-- LCD_STAT INTERRUPT HANDLER --------\\l";
  if (b.type & CodeBlock::TIMER_HANDLER)
    out << ";-- TIMER INTERRUPT HANDLER -----------\\l";
  if (b.type & CodeBlock::SERIAL_HANDLER)
    out << ";-- SERIAL INTERRUPT HANDLER ----------\\l";
  if (b.type & CodeBlock::JOYPAD_HANDLER)
    out << ";-- JOYPAD INTERRUPT HANDLER ----------\\l";

  if (b.type & CodeBlock::ENTRYPOINT)
    out << ";-- ENTRYPOINT ------------------------\\l";

  out << b.name << ":\\l}|{";

  // Print disassembly
  for (CodeBlock::DisassemblyConstIterator i = b.disassembly.begin();
       i != b.disassembly.end(); i++) {
    addrstr << std::hex << "0x" << std::setw(4) << std::setfill('0') << i->first
            << "\\l|";

    CodeBlock::DisassemblyConstIterator j = i;
    // Generate an output port "<out>" for the node at the last instruction
    if (++j == b.disassembly.end())
      inststr << "<out>";
    inststr << i->second << "\\l|";
  }
  string addr = addrstr.str();
  string inst = inststr.str();
  addr[addr.size() - 1] = ' ';
  inst[inst.size() - 1] = ' ';
  out << "{" << addr << "}|{" << inst << "}}}\"] " << b.name << ";" << endl;
}

void GraphDisassemblyOutput::show_xrefs(vector<CodeBlock> &v) {
  for (vector<CodeBlock>::iterator i = v.begin(); i != v.end(); i++) {
    for (CodeBlock::XrefsIterator x = i->xrefs.begin(); x != i->xrefs.end();
         x++) {
      int src = x->first;
      vector<CodeBlock>::iterator src_block = v.end();

      for (vector<CodeBlock>::iterator j = v.begin(); j != v.end(); j++) {
        if (src >= j->start && src < j->end)
          src_block = j;
      }

      if (src_block != v.end()) {
        string color = (i->start < src) ? "red" : "green";
        out << src_block->name << ":out:sw ->" << i->name << ":in:n"
            << "[color=" << color << "];" << endl;
      } else {
        logger.error("Dangling xref src=", std::hex, src, " dst=", i->start);
      }
    }
  }
}

void GraphDisassemblyOutput::show_jump_table_block(GameBoy &gb,
                                                   const CodeBlock &b) {
  /*
     int n = (b.end - b.start)/2;

     out << ";-- JUMP TABLE ------------------------" << endl;
     for (int i=0; i<n; i++)
     {
     address addr = b.start+2*i;
     out << "\t0x" << std::hex << std::setw(4) <<  std::setfill('0') << addr <<
     "\t" <<  i << "\t0x" << std::setw(2) << gb.memory.read16(addr) << endl;;
     }
     */
}

void GraphDisassemblyOutput::generate_output(GameBoy &gb,
                                             vector<CodeBlock> &v) {
  // std::for_each(tmp.begin(), tmp.end(), show_block);

  out << "digraph disassembly {" << endl
      << "splines=polyline;" << endl
      << "node [shape=record, fontname=fixed];" << endl;

  for (vector<CodeBlock>::iterator i = v.begin(); i != v.end(); i++) {
    switch (i->type) {
    case CodeBlock::JUMP_TABLE:
      show_jump_table_block(gb, *i);
      break;
    default:
      show_disassembly_block(*i);
    }
  }

  show_xrefs(v);

  out << "}" << endl;
}
