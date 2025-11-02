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
#ifndef NOMBC_H
#define NOMBC_H

#include "MBC.h"
#include <cstring>

class NoMBC : public MBC {
  u8 ROM[32768];
  u8 RAM[8192];

public:
  NoMBC(GBRom *rom) { memcpy(ROM, rom->data, 32768); }
  u8 read(u16 addr) const;
  u16 read16(u16 addr) const;
  void write(u16 addr, u8 value);

  u32 getUniqueAddress(u16 addr) const { return addr; }
};

#endif
