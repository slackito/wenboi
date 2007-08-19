#include "gbrom.h"
#include "logger.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>

using std::ifstream;
using std::ios;
using std::cout;
using std::endl;

GBRom *read_gbrom(std::string filename)
{
	ifstream is;
	is.open(filename.c_str(), ios::binary);

	is.seekg(0,ios::end);
	int length = is.tellg();
	cout << "Loading " << filename << " (length=" << length << ")" << endl;
	is.seekg(0,ios::beg);

	void *buffer = ::operator new(length);
	is.read((char*)buffer, length);
	GBRom *rom = (GBRom*) buffer;

	char buf[17];
	std::memcpy(buf, rom->header.old_title, 16);
	buf[16]=0;
	
	logger.info("Loaded "+std::string(buf));
	log_rom_header(rom, Logger::DEBUG);

	return rom;
}

void log_rom_header(GBRom *rom, Logger::log_level level)
{
	ostringstream out;

	out.setf(std::ios::hex, std::ios::basefield);
	out << "Entrypoint: " << rom->header.entry_point << endl;
	out << "New licensee code: " << 
		rom->neader.new_licensee_code[0] <<
		rom->neader.new_licensee_code[1] << endl;

	out << "SGB flag: " << rom->header.sgb_flag << endl;
	out << "Cartridge type: " << rom->header.cartridge_type << endl;
	out << "ROM size: " << (32 << rom->header.rom_size) << "K" << endl;
	out << "RAM size: " << rom->header.ram_size << endl;
	out << "Destination code: " << rom->header.destination_code << endl;
	out << "Old licensee code: " << rom->header.old_licensee_code << endl;
	out << "Mask ROM version number: " << rom->header.mask_rom_version_number << endl;
	out << "Header checksum: " << rom->header.header_checksum << endl;
	out << "Global checksum: " << 
		rom->header.global_checksum[0] << 
		rom->header.global_checksum[1] << endl;

	logger.log(out.str(), level);
}

#ifdef TEST_GBROM
int main(int argc, char *argv[])
{
	GBRom *rom=read_gbrom(argv[1]);
}
#endif
