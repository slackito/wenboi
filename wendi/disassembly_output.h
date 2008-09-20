#ifndef DISASSEMBLY_OUTPUT_H
#define DISASSEMBLY_OUTPUT_H

#include "../gbcore.h"
#include "CodeBlock.h"
#include <vector>

class DisassemblyOutput
{
  public:
  virtual void generate_output(GameBoy &gb, std::vector<CodeBlock> &v)=0;
  virtual ~DisassemblyOutput() {}
};

#endif

