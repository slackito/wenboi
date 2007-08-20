#include "GBRom.h"
#include "Logger.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;

void log_rom_header(GBRom *rom, Logger::log_level level)
{
	std::ostringstream out;

	out << "Logging ROM header data:" << endl;

	out << std::hex << std::right << std::setfill('0');
	out << "Entrypoint: 0x" << std::setw(8) << 
		rom->header.entry_point << endl;

	out << "SGB flag: "       << int(rom->header.sgb_flag) << endl;
	out << "Cartridge type: " << int(rom->header.cartridge_type) << endl;
	out << std::dec;
	out << "ROM size: " << (32 << int(rom->header.rom_size)) << "K" << endl;
	out << "RAM size: " << int(rom->header.ram_size) << endl;
	out << std::hex;
	out << "Destination code: 0x"        << std::setw(2) << int(rom->header.destination_code) << endl;
	if (rom->header.old_licensee_code == 0x33) {
		out << "-- ROM uses new licensee code field --" << endl;
		out << "New licensee code: 0x" << std::setw(2) << 
			int(rom->header.new_licensee_code[0]) << std::setw(2) << 
			int(rom->header.new_licensee_code[1]) << endl;
	} else {
		out << "Old licensee code: 0x"       << std::setw(2) << int(rom->header.old_licensee_code) << endl;
	}
	if (rom->header.cgb_flag == 0x80)
		out << "-- ROM supports Color GameBoy functions --" << endl;

	if (rom->header.cgb_flag == 0xC0)
		out << "-- ROM is for Color GameBoy only --" << endl;

	out << "Mask ROM version number: 0x" << std::setw(2) << int(rom->header.mask_rom_version_number) << endl;
	out << "Header checksum: 0x"         << std::setw(2) << int(rom->header.header_checksum) << endl;
	out << "Global checksum: 0x"         << std::setw(2) << 
		int(rom->header.global_checksum[0]) << std::setw(2) <<  
		int(rom->header.global_checksum[1]) << endl;

	logger.log(level, out.str());
}

GBRom *read_gbrom(std::string filename)
{
	ifstream is;
	is.open(filename.c_str(), ios::binary);

	is.seekg(0,ios::end);
	int length = is.tellg();
	cout << "Loading " << filename << " (length=" << length << ")" << endl;
	is.seekg(0,ios::beg);

	void *buffer = ::operator new(length);
	is.read(reinterpret_cast<char*>(buffer), length);
	GBRom *rom = reinterpret_cast<GBRom*>(buffer);

	char buf[17];
	std::memcpy(buf, rom->header.old_title, 16);
	buf[16]=0;
	
	logger.info("Loaded "+std::string(buf));
	log_rom_header(rom, Logger::DEBUG);

	return rom;
}

#ifdef TEST_GBROM
int main(int argc, char *argv[])
{
	GBRom *rom=read_gbrom(argv[1]);
}
#endif
