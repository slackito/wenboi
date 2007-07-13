#ifndef MBC_H
#define MBC_H

#include "sized_types.h"

class MBC
{
	virtual u8 read_byte() const=0;
	virtual void write_byte(u8)=0;
};

#endif // MBC_H
