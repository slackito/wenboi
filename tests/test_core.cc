#include "../gbcore.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstdlib>
#include <vector>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

int main(int argc, char **argv)
{
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " rom_file" << endl;
		exit(EXIT_FAILURE);
	}
	GameBoy gb(argv[1]);
			
	cout << gb.status_string() << endl;

	bool exit_program=false;
	string line, command, last_command;
	vector<string> arguments;
	
	while(!exit_program)
	{
		cout << "(wenboi) ";
		std::getline(cin, line, '\n');
		if (line == "") command = last_command;
		else
		{
			std::istringstream iss(line);
			iss >> command;
			arguments.clear();
			while(iss.good()) {
				string arg;
				iss >> arg;
				arguments.push_back(arg);
			}
		}
		
		if (command == "step") 
		{
			gb.run_cycle();
			cout << gb.status_string() << endl;
		}
		else if (command == "run") 
		{
			gb.run();
		}
		else if (command == "quit")
		{
			exit_program = true;
		}
		else if (command == "disasm")
		{
			int start, end, pos;
			switch(arguments.size())
			{
				case 0:
					start = gb.regs.PC;
					end = start + 30;
					break;
				case 1:
					start = atoi(arguments[0].c_str());
					end = start + 30;
					break;
				case 2:
				default:
					start = atoi(arguments[0].c_str());
					end   = atoi(arguments[1].c_str());
					break;
			}
			
			pos = start;
			while (pos < end)
			{
				string ins;
				int len;
				gb.disassemble_opcode(pos, ins, len);
				cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << 
						pos << "\t";
				for (int i=0; i<len; i++)
					cout << std::setw(2) << int(gb.memory.read(pos+i)) << " ";
			
				if (len < 3) cout << "\t";
				cout << "\t" << ins << endl;
				pos += len;
			}

		}
		else 
		{
			cout << "Unknown command '" << command << "'" << endl;
		}
		
		last_command = command;
	}
}
