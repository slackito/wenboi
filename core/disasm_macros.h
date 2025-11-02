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

// Macros to avoid unnecesary repetition in GameBoy.cc (disassembler)

#ifndef DISASM_MACROS_H
#define DISASM_MACROS_H

#define dis_for_each_register(opA, opB, opC, opD, opE, opH, opL, name, itype,  \
                              macro)                                           \
  macro(opA, name, itype, A) macro(opB, name, itype, B)                        \
      macro(opC, name, itype, C) macro(opD, name, itype, D)                    \
          macro(opE, name, itype, E) macro(opH, name, itype, H)                \
              macro(opL, name, itype, L)

#define dis_for_each_register16(opBC, opDE, opHL, opSP, name, itype, macro)    \
  macro(opBC, name, itype, BC) macro(opDE, name, itype, DE)                    \
      macro(opHL, name, itype, HL) macro(opSP, name, itype, SP)

#define dis(opcode, name, itype)                                               \
  case opcode:                                                                 \
    result << name;                                                            \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = "";                                                              \
    op2.str = "";                                                              \
    op1.type = Instruction::NONE;                                              \
    op2.type = Instruction::NONE;                                              \
    break;

// OP reg
#define dis_reg(opcode, name, itype, reg8)                                     \
  case opcode:                                                                 \
    result << name << " " << #reg8;                                            \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg8;                                                           \
    op2.str = "";                                                              \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg8;                                               \
    op2.type = Instruction::NONE;                                              \
    break;

// OP reg16
#define dis_reg16(opcode, name, itype, reg16)                                  \
  case opcode:                                                                 \
    result << name << " " << #reg16;                                           \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg16;                                                          \
    op2.str = "";                                                              \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg16;                                              \
    op2.type = Instruction::NONE;                                              \
    break;

// OP (reg16)
#define dis__reg16_(opcode, name, itype, reg16)                                \
  case opcode:                                                                 \
    result << name << " (" << #reg16 << ")";                                   \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = std::string("(") + #reg16 + ")";                                 \
    op2.str = "";                                                              \
    op1.type = Instruction::MEM_INDIRECT;                                      \
    op1.reg = Instruction::reg16;                                              \
    op2.type = Instruction::NONE;                                              \
    break;

// OP inm8
#define dis_inm8(opcode, name, itype)                                          \
  case opcode: {                                                               \
    int inm = int(this->memory.read(PC++, GBMemory::DONT_WATCH));              \
    result << name << " 0x" << std::setw(2) << inm;                            \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = toString(inm);                                                   \
    op2.str = "";                                                              \
    op1.type = Instruction::INM8;                                              \
    op1.val = inm;                                                             \
    op2.type = Instruction::NONE;                                              \
    break;                                                                     \
  }

// OP inm16
#define dis_inm16(opcode, name, itype)                                         \
  case opcode: {                                                               \
    int inm = int(this->memory.read16(PC, GBMemory::DONT_WATCH));              \
    PC += 2;                                                                   \
    result << name << " 0x" << std::setw(4) << inm;                            \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = toString(inm);                                                   \
    op2.str = "";                                                              \
    op1.type = Instruction::INM16;                                             \
    op1.val = inm;                                                             \
    op2.type = Instruction::NONE;                                              \
    break;                                                                     \
  }

#define dis_reg_inm(opcode, name, itype, reg8)                                 \
  case opcode: {                                                               \
    int inm = int(this->memory.read(PC++, GBMemory::DONT_WATCH));              \
    result << name << " " << #reg8 << ", 0x" << std::setw(2) << inm;           \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg8;                                                           \
    op2.str = toString(inm);                                                   \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg8;                                               \
    op2.type = Instruction::INM8;                                              \
    op2.val = inm;                                                             \
    break;                                                                     \
  }

#define dis_reg16_inm(opcode, name, itype, reg16)                              \
  case opcode: {                                                               \
    int inm = int(this->memory.read16(PC, GBMemory::DONT_WATCH));              \
    PC += 2;                                                                   \
    result << name << " " << #reg16 << ", 0x" << std::setw(4) << inm;          \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg16;                                                          \
    op2.str = toString(inm);                                                   \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg16;                                              \
    op2.type = Instruction::INM16;                                             \
    op2.val = inm;                                                             \
    break;                                                                     \
  }

#define dis_reg16_inm8(opcode, name, itype, reg16)                             \
  case opcode: {                                                               \
    int inm = int(this->memory.read(PC++, GBMemory::DONT_WATCH));              \
    result << name << " " << #reg16 << ", 0x" << std::setw(2) << inm;          \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg16;                                                          \
    op2.str = toString(inm);                                                   \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg16;                                              \
    op2.type = Instruction::INM8;                                              \
    op2.val = inm;                                                             \
    break;                                                                     \
  }

#define dis_reg_reg(opcode, name, itype, reg1, reg2)                           \
  case opcode:                                                                 \
    result << name << " " << #reg1 << ", " << #reg2;                           \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg1;                                                           \
    op2.str = #reg2;                                                           \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg1;                                               \
    op2.type = Instruction::REG;                                               \
    op2.val = Instruction::reg2;                                               \
    break;

#define dis_A_reg(opcode, name, itype, reg2)                                   \
  dis_reg_reg(opcode, name, itype, A, reg2)
#define dis_B_reg(opcode, name, itype, reg2)                                   \
  dis_reg_reg(opcode, name, itype, B, reg2)
#define dis_C_reg(opcode, name, itype, reg2)                                   \
  dis_reg_reg(opcode, name, itype, C, reg2)
#define dis_D_reg(opcode, name, itype, reg2)                                   \
  dis_reg_reg(opcode, name, itype, D, reg2)
#define dis_E_reg(opcode, name, itype, reg2)                                   \
  dis_reg_reg(opcode, name, itype, E, reg2)
#define dis_H_reg(opcode, name, itype, reg2)                                   \
  dis_reg_reg(opcode, name, itype, H, reg2)
#define dis_L_reg(opcode, name, itype, reg2)                                   \
  dis_reg_reg(opcode, name, itype, L, reg2)

#define dis_reg16_reg16(opcode, name, itype, reg16_1, reg16_2)                 \
  case opcode:                                                                 \
    result << name << " " << #reg16_1 << ", " << #reg16_2;                     \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg16_1;                                                        \
    op2.str = #reg16_2;                                                        \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg16_1;                                            \
    op2.type = Instruction::REG;                                               \
    op2.val = Instruction::reg16_2;                                            \
    break;

#define dis_HL_reg16(opcode, name, itype, reg16_2)                             \
  dis_reg16_reg16(opcode, name, itype, HL, reg16_2)

// OP reg, (reg16)
#define dis_reg__reg16_(opcode, name, itype, reg8, reg16)                      \
  case opcode:                                                                 \
    result << name << " " << #reg8 << ", (" << #reg16 << ")";                  \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg8;                                                           \
    op2.str = std::string("(") + #reg16 + ")";                                 \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg8;                                               \
    op2.type = Instruction::MEM_INDIRECT;                                      \
    op2.val = Instruction::reg16;                                              \
    break;

// OP reg, (HL)
#define dis_reg__HL_(opcode, name, itype, reg)                                 \
  dis_reg__reg16_(opcode, name, itype, reg, HL)

// OP reg, (inm)
#define dis_reg__inm_(opcode, name, itype, reg8)                               \
  case opcode: {                                                               \
    int inm = int(this->memory.read16(PC, GBMemory::DONT_WATCH));              \
    PC += 2;                                                                   \
    result << name << " " << #reg8 << ", (0x" << std::setw(4) << inm << ")";   \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = #reg8;                                                           \
    op2.str = std::string("(") + toString(inm) + ")";                          \
    op1.type = Instruction::REG;                                               \
    op1.reg = Instruction::reg8;                                               \
    op2.type = Instruction::MEM_DIRECT;                                        \
    op2.val = inm;                                                             \
    break;                                                                     \
  }

// OP (reg16), reg
#define dis__reg16__reg(opcode, name, itype, reg16, reg8)                      \
  case opcode:                                                                 \
    result << name << " (" << #reg16 << "), " << #reg8;                        \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = std::string("(") + #reg16 + ")";                                 \
    op2.str = #reg8;                                                           \
    op1.type = Instruction::MEM_INDIRECT;                                      \
    op1.val = Instruction::reg16;                                              \
    op2.type = Instruction::REG;                                               \
    op2.reg = Instruction::reg8;                                               \
    break;

// OP (HL), reg
#define dis__HL__reg(opcode, name, itype, reg)                                 \
  dis__reg16__reg(opcode, name, itype, HL, reg)

// OP (inm), reg
#define dis__inm__reg(opcode, name, itype, reg8)                               \
  case opcode: {                                                               \
    int inm = int(this->memory.read16(PC, GBMemory::DONT_WATCH));              \
    PC += 2;                                                                   \
    result << name << " (0x" << std::setw(4) << inm << "), " << #reg8;         \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = std::string("(") + toString(inm) + ")";                          \
    op2.str = #reg8;                                                           \
    op1.type = Instruction::MEM_DIRECT;                                        \
    op1.val = inm;                                                             \
    op2.type = Instruction::REG;                                               \
    op2.reg = Instruction::reg8;                                               \
    break;                                                                     \
  }

// OP (inm), reg16
#define dis__inm__reg16(opcode, name, itype, reg16)                            \
  case opcode: {                                                               \
    int inm = int(this->memory.read16(PC, GBMemory::DONT_WATCH));              \
    PC += 2;                                                                   \
    result << name << " (0x" << std::setw(4) << inm << "), " << #reg16;        \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = std::string("(") + toString(inm) + ")";                          \
    op2.str = #reg16;                                                          \
    op1.type = Instruction::MEM_DIRECT;                                        \
    op1.val = inm;                                                             \
    op2.type = Instruction::REG;                                               \
    op2.reg = Instruction::reg16;                                              \
    break;                                                                     \
  }

// OP (reg16), inm
#define dis__reg16__inm(opcode, name, itype, reg16)                            \
  case opcode: {                                                               \
    int inm = int(this->memory.read(PC++, GBMemory::DONT_WATCH));              \
    result << name << " (" << #reg16 << "), 0x" << std::setw(2) << inm;        \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = std::string("(") + #reg16 + ")";                                 \
    op2.str = toString(inm);                                                   \
    op1.type = Instruction::MEM_INDIRECT;                                      \
    op1.reg = Instruction::reg16;                                              \
    op2.type = Instruction::INM8;                                              \
    op2.val = inm;                                                             \
    break;                                                                     \
  }

// Special routine for JR
#define dis_JR(opcode, name, itype)                                            \
  case opcode: {                                                               \
    s8 offset = this->memory.read(PC++);                                       \
    result << name << " " << std::dec << int(offset) << "\t[0x" << std::hex    \
           << std::setw(2) << int(PC + offset) << "]";                         \
    opcode_str = name;                                                         \
    ins_type = itype;                                                          \
    op1.str = toString(int(offset));                                           \
    op2.str = toString(int(PC + offset));                                      \
    op1.type = Instruction::INM8;                                              \
    op1.val = offset;                                                          \
    op2.type = Instruction::NONE;                                              \
    op2.val = int(PC + offset);                                                \
    break;                                                                     \
  }

////////////////////////////////////////////////////////////

#endif
