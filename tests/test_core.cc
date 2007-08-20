#include "../gbcore.h"
#include <iostream>

int main(int argc, char **argv)
{
	GameBoy gb(argv[1]);
	while(1)
	{
		gb.run_cycle();
	}


}
