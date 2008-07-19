#include "MBC.h"
#include "NoMBC.h"
#include "MBC1.h"

#include "Logger.h"

MBC *create_MBC(GBRom *rom)
{
	switch (rom->header.cartridge_type)
	{
		case cartridge_types::ROM_ONLY:		return new NoMBC(rom);
		case cartridge_types::MBC1:			return new MBC1(rom);
		default:
			logger.critical("Unsupported cartridge type ", 
					int(rom->header.cartridge_type));
			return 0;
	}
}

