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

int str2int(string s)
{
	if (s[0] == '0') {
		if (s[1] == 'x')
			return strtol(s.c_str()+2, NULL, 16);
		else if (s[1] >= '0' && s[1] <= '9')
			return strtol(s.c_str()+1, NULL, 8);
		else return 0;
	}
	return strtol(s.c_str(), NULL, 10);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		cerr << "Usage: " << argv[0] << " rom_file" << endl;
		exit(EXIT_FAILURE);
	}
	GameBoy gb(argv[1]);
			
	cout << gb.status_string() << endl;

	string line, command, last_command;
	vector<string> arguments;
	
	while(true)
	{
		cout << "(wenboi) ";
		std::getline(cin, line, '\n');
		if (!cin.good()) break; // if stdin is closed, exit main loop
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
		
		if (command == "step" || command == "s") 
		{
			gb.run_cycle();
			cout << gb.status_string() << endl;
		}
		else if (command == "run" || command == "r") 
		{
			int status = gb.run();
			if (status == GameBoy::QUIT)
			{
				break;
			}
			else if (status == GameBoy::BREAKPOINT)
			{
				cout << "Breakpoint hit at " << gb.regs.PC << endl;
				cout << gb.status_string() << endl;
			}
			else
			{
				cout << "run returned with status " << status << endl;
			}
		}
		else if (command == "quit" || command == "q")
		{
			break;
		}
		else if (command == "disasm" || command == "d")
		{
			int start, end, pos;
			switch(arguments.size())
			{
				case 0:
					start = gb.regs.PC;
					end = start + 30;
					break;
				case 1:
					start = str2int(arguments[0]);
					end = start + 30;
					break;
				case 2:
				default:
					start = str2int(arguments[0]);
					end   = str2int(arguments[1]);
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
		else if (command == "x")
		{
			int addr = str2int(arguments[0]);
			cout << "[" << std::hex << std::setw(4) << std::setfill('0') <<
				addr << "]\t";
			if (arguments.size() > 1)
			{
				if (arguments[1] == "d")
					cout << std::dec << int(gb.memory.read(addr)) << endl;
			}
			cout<< int(gb.memory.read(addr)) << endl;
		}
		else if (command == "break" || command == "b")
		{
			int addr = str2int(arguments[0]);
			cout << "breakpoint #" << gb.set_breakpoint(addr) <<
				" set at 0x" << std::hex << std::setw(4) << std::setfill('0') << addr << endl;
		}
		else if (command == "delete")
		{
			gb.delete_breakpoint(str2int(arguments[0]));
		}
		else if (command == "enable")
		{
			gb.enable_breakpoint(str2int(arguments[0]));
		}
		else if (command == "disable")
		{
			gb.disable_breakpoint(str2int(arguments[0]));
		}
		else 
		{
			cout << "Unknown command '" << command << "'" << endl;
		}
		
		last_command = command;
	}
}
