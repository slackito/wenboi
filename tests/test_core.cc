/*
    Copyright 2008 Jorge Gorbe Moya <slack@codemaniacs.com>

    This file is part of wenboi 

    wenboi is free software: you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 3 only, as published by the
    Free Software Foundation.

    wenboi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wenboi.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "../gbcore.h"
#include "../Logger.h"
#include "../wendi/disasm.h"
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

void print_run_result(GameBoy &gb, int status)
{
	if (status == GameBoy::BREAKPOINT)
	{
		cout << "Breakpoint hit at " << gb.regs.PC << endl;
		cout << gb.status_string() << endl;
	}
	else if (status == GameBoy::WATCHPOINT)
	{
		cout << "Watchpoint 0x" << std::hex << std::setw(4) << std::setfill('0') <<
			int(gb.memory.watchpoint_addr) << " hit at 0x" << gb.regs.PC;
		if (gb.memory.watchpoint_newvalue == 0xFFFF)
		{
			cout << " (READ)" << endl << 
				"value = " << int(gb.memory.watchpoint_oldvalue) << endl;
		}
		else
		{
			cout << " (WRITE)" << endl <<
				"old = " << int(gb.memory.watchpoint_oldvalue) << 
				" new = " << int(gb.memory.watchpoint_newvalue) << endl;
		}
	}
	//else
	//{
	//	cout << "run returned with status " << status << endl;
	//}
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " rom_file" << endl;
		exit(EXIT_FAILURE);
	}
	GameBoy gb(argv[1]);
			
	cout << gb.status_string() << endl;

	string line, command, last_command;
	vector<string> arguments;
	bool first_cmd = false;

	if (argc == 2)
	{
		first_cmd = true;
		command = "run";
	}
	
	while(true)
	{
		if (first_cmd)
		{
			first_cmd = false;
		}
		else
		{
			cout << "(wenboi) ";
			cout.flush();
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
		}


		if (command == "step" || command == "s") 
		{
			int status;
			while((status = gb.run_cycle()) == GameBoy::WAIT) {} // do nothing
			
			print_run_result(gb, status);
			cout << gb.status_string() << endl;
		}
		else if (command == "run" || command == "r" || command == "cont") 
		{
			if (command != "cont")
			{
				gb.reset();
			}
			if (arguments.size() == 0)
			{
				int status = gb.run();
				if (status == GameBoy::QUIT)
				{
					break;
				}
				print_run_result(gb, status);
			}
			else if (arguments.size() == 1)
			{
				int cycles = str2int(arguments[0]);

				for (u32 i=0; i<cycles/gb.CYCLE_STEP; i++)
					gb.run_cycle();
				
				cout << gb.status_string() << endl;
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
				Instruction ins(disassemble_opcode(gb, pos));
				cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << 
						pos << "\t";
				for (int i=0; i<ins.length; i++)
					cout << std::setw(2) << int(gb.memory.read(pos+i)) << " ";
			
				if (ins.length < 3) cout << "\t";
				cout << "\t" << ins.all << endl;
				pos += ins.length;
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
		else if (command == "write")
		{
			int addr = str2int(arguments[0]);
			u8 value = str2int(arguments[1]);

			gb.memory.write(addr, value);
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
		else if (command == "watch" || command == "b")
		{
			int addr = str2int(arguments[0]);
			cout << "watchpoint #" << gb.memory.set_watchpoint(addr) <<
				" set at 0x" << std::hex << std::setw(4) << std::setfill('0') << addr << endl;
		}
		else if (command == "wdelete")
		{
			gb.memory.delete_watchpoint(str2int(arguments[0]));
		}
		else if (command == "wenable")
		{
			gb.memory.enable_watchpoint(str2int(arguments[0]));
		}
		else if (command == "wdisable")
		{
			gb.memory.disable_watchpoint(str2int(arguments[0]));
		}
		else if (command == "display")
		{
			if (arguments[0] == "bgmap")
				gb.video.set_display_mode(GBVideo::BG_MAP);
			else if (arguments[0] == "wmap")
				gb.video.set_display_mode(GBVideo::WINDOW_MAP);
			else if (arguments[0] == "smap")
				gb.video.set_display_mode(GBVideo::SPRITE_MAP);
			else
				gb.video.set_display_mode(GBVideo::NORMAL);
		}
		else if (command == "logger")
		{
			if (arguments[0] == "critical")
				logger.set_log_level(Logger::CRITICAL);
			else if (arguments[0] == "error")
				logger.set_log_level(Logger::ERROR);
			else if (arguments[0] == "warning")
				logger.set_log_level(Logger::WARNING);
			else if (arguments[0] == "info")
				logger.set_log_level(Logger::INFO);
			else if (arguments[0] == "debug")
				logger.set_log_level(Logger::DEBUG);
			else if (arguments[0] == "trace")
				logger.set_log_level(Logger::TRACE);
		}
		else if (command == "reset")
		{
			gb.reset();
		}
		else 
		{
			cout << "Unknown command '" << command << "'" << endl;
		}
		
		last_command = command;
	}
}
