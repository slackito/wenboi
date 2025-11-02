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
#include "MBC1.h"
#include "../common/Logger.h"

#include <cassert>
#include <iomanip>

u8 MBC1::read(u16 addr) const {
  if (addr <= 0x3FFF) // ROM Bank 0
    return ROM[addr];
  else if (addr <= 0x7FFF) // ROM (switchable)
  {
    u8 rom_bank = rom_bank_low;
    if (mode == ROM_BANKING_MODE)
      rom_bank |= (ram_bank << 5);

    u32 base = 16384 * rom_bank;
    return ROM[base + (addr - 0x4000)];
  } else // if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
  {
    if (ram_enabled) {
      u32 base = (mode == RAM_BANKING_MODE ? 8192 * ram_bank : 0);
      return RAM[base + (addr - 0xA000)];
    } else
      return 0xFF;
  }
  // else
  //	logger.error("MBC1: Incorrect read");
  return 0;
}

u16 MBC1::read16(u16 addr) const {
  assert(addr != 0x3FFF);
  assert(addr != 0x7FFF);
  assert(addr != 0xBFFF);

  if (addr <= 0x3FFF) // ROM Bank 0
    return ROM[addr] | (ROM[addr + 1] << 8);
  else if (addr <= 0x7FFF) // ROM (switchable)
  {
    u8 rom_bank = rom_bank_low;
    if (mode == ROM_BANKING_MODE)
      rom_bank |= (ram_bank << 5);

    u32 offset = 16384 * rom_bank + (addr - 0x4000);
    return ROM[offset] | (ROM[offset + 1] << 8);
  } else // if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
  {
    if (ram_enabled) {
      u32 base = (mode == RAM_BANKING_MODE ? 8192 * ram_bank : 0);
      u32 offset = base + (addr - 0xA000);
      return RAM[offset] | (RAM[offset + 1] << 8);
    } else
      return 0xFFFF;
  }
  // else
  //	logger.error("MBC1: Incorrect read");
  return 0;
}

void MBC1::write(u16 addr, u8 value) {
  if (addr <= 0x1FFF) {
    if ((value & 0x0F) == 0x0A)
      ram_enabled = true;
    else
      ram_enabled = false;
  } else if (addr <= 0x3FFF) {
    rom_bank_low = value & 0x1F;
    if (rom_bank_low == 0)
      rom_bank_low = 1;
  } else if (addr <= 0x5FFF) {
    ram_bank = value & 0x03;
  } else if (addr <= 0x7FFF) {
    mode = BankingMode(value & 1);
  } else if ((addr & 0xE000) == 0xA000) //(addr >= 0xA000 && addr <= 0xBFFF)
  {
    u32 base = (mode == RAM_BANKING_MODE ? 8192 * ram_bank : 0);
    RAM[base + (addr - 0xA000)] = value;
    return;
  } else {
    logger.debug("MBC1: trying to write in ROM, addr=0x", std::hex, addr);
  }
}

u32 MBC1::getUniqueAddress(u16 addr) const {
  if (addr <= 0x3FFF) // ROM Bank 0
    return addr;
  else if (addr <= 0x7FFF) // ROM (switchable)
  {
    u8 rom_bank = rom_bank_low;
    if (mode == ROM_BANKING_MODE)
      rom_bank |= (ram_bank << 5);

    u32 base = 16384 * rom_bank;
    return base + (addr - 0x4000);
  } else // if ((addr&0xE000) == 0xA000)   //(addr >= 0xA000 && addr <= 0xBFFF)
  {
    if (ram_enabled) {
      u32 base = (mode == RAM_BANKING_MODE ? 8192 * ram_bank : 0);
      return (base + (addr - 0xA000)) | 0x80000000;
    } else
      return 0;
  }
  return 0;
}
