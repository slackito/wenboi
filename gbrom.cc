#include "gbrom.h"
#include <fstream>
#include <iostream>
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
	cout << "Loaded " << buf << endl;
}

#ifdef TEST_GBROM
int main(int argc, char *argv[])
{
	GBRom *rom=read_gbrom(argv[1]);
}
#endif
