#ifndef OUTPUT_TXT
#define OUTPUT_TXT
#include "disassembly_output.h"
#include "CodeBlock.h"
#include <vector>
#include <iostream>

class TextDisassemblyOutput: public DisassemblyOutput
{
  std::ostream &out;
  
  void show_disassembly_block(const CodeBlock& b);
  void show_jump_table_block(GameBoy &gb, const CodeBlock &b);
  void hexdump(GameBoy &gb, address start, address end);

  public:
  TextDisassemblyOutput(std::ostream &ofs): out(ofs) {}
  void generate_output(GameBoy &gb, std::vector<CodeBlock> &v);
};

#endif

