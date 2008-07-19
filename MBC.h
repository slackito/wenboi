#ifndef MBC_H
#define MBC_H

#include "sized_types.h"
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
