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
#include "util.h"

uint32 set_bit(uint32 val, uint32 pos)
{
	uint32 mask = 1<<pos;
	return val | mask;
}

uint32 reset_bit(uint32 val, uint32 pos)
{
	uint32 mask = ~(1<<pos);
	return val & mask;
}

uint32 flip_bit(uint32 val, uint32 pos)
{
	uint32 mask = 1<<pos;
	return val ^ mask;
}

bool check_bit(uint32 val, uint32 pos)
{
	uint32 mask = 1<<pos;
	return ((val&mask) != 0);
}

