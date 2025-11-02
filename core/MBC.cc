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
#include "MBC.h"
#include "MBC1.h"
#include "NoMBC.h"

#include "../common/Logger.h"

MBC *create_MBC(GBRom *rom) {
  switch (rom->header.cartridge_type) {
  case cartridge_types::ROM_ONLY:
    return new NoMBC(rom);
  case cartridge_types::MBC1:
    return new MBC1(rom);
  default:
    logger.critical("Unsupported cartridge type ",
                    int(rom->header.cartridge_type));
    return 0;
  }
}
