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
#ifndef MBC_H
#define MBC_H

#include "../common/sized_types.h"
#include "GBRom.h"

class MBC
{
	public:
	virtual u8   read  (u16 addr) const=0;
	virtual u16  read16(u16 addr) const=0;
	virtual void write (u16 addr, u8 value)=0;
	virtual ~MBC() {};
};


MBC *create_MBC(GBRom *rom);

#endif // MBC_H
