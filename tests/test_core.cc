#include "../gbcore.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " rom_file" << std::endl;
		exit(EXIT_FAILURE);
	}
	GameBoy gb(argv[1]);
	while(1)
	{
		gb.run_cycle();
	}
}
