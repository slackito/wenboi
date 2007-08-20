#ifndef MBC_H
#define MBC_H

#include "sized_types.h"
#include "GBRom.h"

class MBC
{
	public:
	virtual u8   read(int addr) const=0;
	virtual void write(int addr, u8 value)=0;
	virtual ~MBC() {};
};

class NoMBC: public MBC
{
	u8 ROM[32768];
	u8 RAM[8192];
	
	public:
	NoMBC(GBRom *rom) { memcpy(ROM, rom->data, 32768); }
	u8   read (int addr) const;
	void write(int addr, u8 value);

};


MBC *create_MBC(GBRom *rom);

#endif // MBC_H
