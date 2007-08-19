#include "MBC.h"
#include "logger.h"

using namespace cartridge_types;

MBC *create_MBC(GBRom *rom)
{
	switch (rom->header.cartridge_type)
	{
		case ROM_ONLY:		return new NoMBC(rom);
		//case MBC1:		return new MBC1(rom);
		default:
			logger.critical("Unsupported cartridge type");
			return 0;
	}
}
